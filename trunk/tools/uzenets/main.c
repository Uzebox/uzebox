/*
    uzenets - Network server for uzenet dongles
    Copyright (C) 2013  Håkon Nessjøen <haakon.nessjoen@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/* vim: tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */
#if defined(__linux__) || defined(__APPLE__)
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#elif __MINGW32__
#define WIN32_LEAN_AND_MEAN
#include "winsock2.h"
#endif
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

/* Receiving socket */
static int insock;

#ifdef __MINGW32__
WSADATA wsaData;
SOCKET insock;
#endif

#define IPV4_ALEN 4

#define MAX_CONNECTIONS 256
/* Bytes needed for holding max_connections number */
#define MAX_CONNECTIONS_SIZE 1

#define MAX_OUTGOING_QUEUE 256

/* NB!: Changing this value changes the protocol specification */
#define MAX_DONGLE_NAME 16

enum un_connection_state {
	UNCS_NEW,
	UNCS_INIT,
	UNCS_WAITING,
	UNCS_TUNNEL,
};

struct un_connection {
	int16_t timestamp;
	struct sockaddr_in addr;
	struct sockaddr_in opponent;
	uint8_t last_sequence_no;
	uint8_t local_sequence_no;
	uint8_t inuse;
	uint8_t state;
	int8_t dongle_name[16];
	uint16_t game_id;
};

struct un_control_packet {
	uint8_t	cmd;
	uint8_t sequence_no;
	uint16_t packetlen;
};

struct un_outgoing_queue {
	struct un_connection *conn;
	struct un_control_packet packet;
	uint8_t inuse;
	uint8_t sent;
	int16_t timestamp;
};

struct un_joypad {
	uint8_t type;
	uint32_t joypad_status;
};

enum un_control_cmd {
	UNC_INVALID,
	UNC_PING,
	UNC_ACK,
	UNC_INIT,
	UNC_RESULT,
	UNC_REGISTER,
	UNC_FINDSERVERS,
	UNC_STARTGAME,
	UNC_READWRITEJOY,
	UNC_SENDDATA,
	UNC_CLOSE,
	UNC_NUMCMD /* Number of commands, must always be last entry */
};

char *un_control_cmd_names[] = {
	"UNC_INVALID",
	"UNC_PING",
	"UNC_ACK",
	"UNC_INIT",
	"UNC_RESULT",
	"UNC_REGISTER",
	"UNC_FINDSERVERS",
};

int16_t un_packet_sizes[] = {
	-1, /* UNC_INVALID */
	0,  /* UNC_PING */
	0,  /* UNC_ACK */
	0,  /* UNC_INIT */
	-1, /* UNC_RESULT */
	MAX_DONGLE_NAME+2,  /* UNC_REGISTER */
	2,  /* UNC_FINDSERVERS */
	0,  /* UNC_STARTGAME */
	4,  /* UNC_READWRITEJOY */
	-1, /* UNC_SENDDATA */
	1,  /* UNC_CLOSE */
};

struct un_connection connections[MAX_CONNECTIONS];
struct un_outgoing_queue outgoing_queue[MAX_OUTGOING_QUEUE];

struct un_control_packet *check_header(char *buffer, int len) {
	struct un_control_packet *header = (struct un_control_packet *)buffer;

	if (len < sizeof(struct un_control_packet))
		return NULL;

	header->packetlen = htons(header->packetlen);

	if (len != header->packetlen)
		return NULL;

	return header;
}

int check_sequence(struct un_control_packet *header, uint8_t sequence_no) {
	if (header == NULL)
		return 0;

	if (header->sequence_no == (uint8_t)(sequence_no + 1)) {
		return 1;
	}

	return 0;
}

int check_packet_len(struct un_control_packet *header) {
	if (header == NULL)
		return 0;

	if (header->cmd < UNC_NUMCMD) {
		if (un_packet_sizes[header->cmd] == -1)
			return 1;

		if (header->packetlen == un_packet_sizes[header->cmd] + sizeof(struct un_control_packet))
			return 1;
	}
	return 0;
}

int add_connection(struct sockaddr_in *source) {
	int i;

	for (i = MAX_CONNECTIONS; i--;){
		if (connections[i].inuse == 0) {
			connections[i].inuse = 1;
			memcpy(&connections[i].addr, source, sizeof(struct sockaddr_in));
			connections[i].timestamp = time(NULL);
			return i;
		}
	}

	return -1;
}

int find_connection(struct sockaddr_in *source) {
	int i;

	for (i = MAX_CONNECTIONS; i--;){
		if (connections[i].inuse == 1 &&
		  memcmp((char *)&connections[i].addr.sin_addr, (char *)&source->sin_addr, sizeof(connections[i].addr.sin_addr)) == 0 &&
		  memcmp((char *)&connections[i].addr.sin_port, (char *)&source->sin_port, sizeof(connections[i].addr.sin_port)) == 0) {
			return i;
		}
	}

	return -1;
}

int replyto(struct un_connection *conn, char *buffer, int bufferlen) {
	struct sockaddr_in socket_address;
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = conn->addr.sin_port;
	socket_address.sin_addr.s_addr = conn->addr.sin_addr.s_addr;

	int bytes = sendto(insock, buffer, sizeof(struct un_control_packet), 0, (struct sockaddr*)&socket_address, sizeof(socket_address));
	//printf("Sent %d bytes to %s\n", bytes, inet_ntoa(conn->addr.sin_addr));
	printf("< %s\n", un_control_cmd_names[(int)buffer[0]]);
	return bytes;
}

/* resend up to 5 times, until ack is returned */
int replyto_queue(struct un_connection *conn, char *buffer, int bufferlen) {
	int i;

	replyto(conn, buffer, bufferlen);
	for (i = MAX_OUTGOING_QUEUE; --i;) {
		if (outgoing_queue[i].inuse == 0) {
			outgoing_queue[i].inuse = 1;
			outgoing_queue[i].conn = conn;
			memcpy(&outgoing_queue[i].packet, buffer, bufferlen);
			outgoing_queue[i].timestamp = time(NULL) + 1; /* Sent in the future! */
			return i;
		}
	}
	fprintf(stderr, "Error; sendqueue full!\n");
	return -1;
}

void ack_command(struct un_connection *conn) {
	char buffer[100];
	struct un_control_packet *header = (struct un_control_packet *)&buffer;

	header->cmd = UNC_ACK;
	header->sequence_no = conn->last_sequence_no;
	header->packetlen = htons(sizeof(struct un_control_packet));

	replyto(conn, buffer, sizeof(struct un_control_packet));
}

void command_ping(struct un_connection *conn, char *buffer, int bufferlen) {
	ack_command(conn);
}

void command_init(struct un_connection *conn, char *buffer, int bufferlen) {

	conn->state = UNCS_INIT;
	memset(&conn->opponent, 0, sizeof(conn->opponent));
	ack_command(conn);

	struct un_control_packet packet;
	packet.cmd = UNC_INIT;
	packet.sequence_no = ++conn->local_sequence_no;
	packet.packetlen = htons(sizeof(packet));

	replyto_queue(conn, (char *)&packet, sizeof(packet));
}

void command_register(struct un_connection *conn, char *buffer, int bufferlen) {
	struct un_control_packet *packet = (struct un_control_packet *)(buffer - sizeof(struct un_control_packet));
	uint16_t *game_id;
	buffer[bufferlen-1] = '\0';
	printf("Registering client: %s\n", buffer + 2);

	game_id = (uint16_t *)buffer;
	conn->game_id = htons(*game_id);
	memcpy(&conn->dongle_name, buffer + 2, bufferlen - 2);

	ack_command(conn);

	packet->packetlen = sizeof(*packet);
	packet->sequence_no = ++conn->local_sequence_no;
	replyto_queue(conn, (char *)packet, sizeof(*packet));
}

/* Handle ACKs for our queued packets */
void command_ack(struct un_connection *conn, char *buffer, int bufferlen) {
	int i;
	struct un_control_packet *packet = (struct un_control_packet *)(buffer - sizeof(struct un_control_packet));

	for (i = MAX_OUTGOING_QUEUE; i--;) {
		struct un_outgoing_queue *entry = &outgoing_queue[i];
		if (entry->inuse && entry->conn == conn && packet->sequence_no == entry->packet.sequence_no) {
			/* Packet acked, remove from queue */
			entry->inuse = 0;
		}
	}
}

#define FINDSERVERS_PITCH ((MAX_CONNECTIONS_SIZE)+MAX_DONGLE_NAME)
void command_findservers(struct un_connection *conn, char *buffer, int bufferlen) {
	int i;
	uint8_t count;
	uint16_t *game_id = (uint16_t *)buffer;
	uint8_t data[sizeof(struct un_connection)+(FINDSERVERS_PITCH*5)];
	uint8_t *ptr=(uint8_t *)&data + sizeof(struct un_control_packet) + 1; /* +1 = first byte is count */
	struct un_control_packet *packet = (struct un_control_packet *)&data;

	for (i = MAX_CONNECTIONS; i--;) {
		struct un_connection *entry = &connections[i];
		if (entry->inuse && entry != conn && *game_id != 0 && entry->game_id == htons(*game_id)) {
			memcpy(ptr, &i, MAX_CONNECTIONS_SIZE);
			ptr += MAX_CONNECTIONS_SIZE;
			memcpy(ptr, conn->dongle_name, MAX_DONGLE_NAME);
			ptr += MAX_DONGLE_NAME;

			printf("Sending info about server: %s\n", entry->dongle_name);

			if (++count == 5)
				break;
		}
	}
	data[sizeof(struct un_connection)] = count;
	printf("Sending %d servers\n", count);
	printf("Packet size: %d\n", sizeof(struct un_control_packet)+(FINDSERVERS_PITCH * count));

	packet->sequence_no = ++conn->local_sequence_no;
	packet->cmd = UNC_FINDSERVERS;
	packet->packetlen = htons(sizeof(struct un_control_packet)+(FINDSERVERS_PITCH * count));

	replyto_queue(conn, (char *)&data, sizeof(struct un_control_packet)+(FINDSERVERS_PITCH * count));
}

void handle_connection(struct un_connection *conn, char *buffer, int bufferlen) {
	struct un_control_packet *header;
	char *nextbuffer = buffer + sizeof(struct un_control_packet);
	int nextbufferlen = bufferlen - sizeof(struct un_control_packet);

	if ((header = check_header(buffer, bufferlen)) == NULL) {
		fprintf(stderr, "Invalid header in packet from %s\n", inet_ntoa(conn->addr.sin_addr));
		return;
	}

	/* UNC_INIT can reset sequence no */
	if (header->cmd != UNC_INIT && header->cmd != UNC_ACK && !check_sequence(header, conn->last_sequence_no)) {
		fprintf(stderr, "Out of order sequence number in packet from %s\n", inet_ntoa(conn->addr.sin_addr));
		return;
	}
	/* ACKS have our own sequence no */
	else if (header->cmd != UNC_ACK) {
		conn->last_sequence_no = header->sequence_no;
	}

	if (!check_packet_len(header)) {
		fprintf(stderr, "Invalid packet length in packet from %s\n", inet_ntoa(conn->addr.sin_addr));
		return;
	}

	printf("> %s\n", un_control_cmd_names[header->cmd]);
	switch (header->cmd) {
		case UNC_ACK:
			command_ack(conn, nextbuffer, nextbufferlen);
			break;

		case UNC_PING:
			command_ping(conn, nextbuffer, nextbufferlen);
			break;

		case UNC_INIT:
			command_init(conn, nextbuffer, nextbufferlen);
			break;

		case UNC_REGISTER:
			command_register(conn, nextbuffer, nextbufferlen);
			break;

		case UNC_FINDSERVERS:
			command_findservers(conn, nextbuffer, nextbufferlen);
			break;

		default:
			ack_command(conn);
	}
}

int main(int argc, char **argv) {
	static struct in_addr sourceip; 
	static int sourceport;
	struct sockaddr_in si_me;
	int optval = 1;

	memset(&connections, 0, sizeof(connections));
	memset(&outgoing_queue, 0, sizeof(outgoing_queue));

#ifdef __MINGW32__
	if ((retval = WSAStartup(0x202, &wsaData)) != 0) {
		fprintf(stderr, "Server: WSAStartup() failed with error %d\n", retval);
		WSACleanup();
		return 1;
	}
#endif

	printf("Opening sockets\n");
	insock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (insock < 0) {
		perror("insock");
		return 1;
	}

	sourceport = 8936;
	inet_pton(AF_INET, (char *)"0.0.0.0", &sourceip);

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(sourceport);
	memcpy(&(si_me.sin_addr), &sourceip, IPV4_ALEN);

	setsockopt(insock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));
	if (bind(insock, (struct sockaddr *)&si_me, sizeof(si_me))==-1) {
		fprintf(stderr, "Error binding to %s:%d, %s\n", inet_ntoa(si_me.sin_addr), sourceport, strerror(errno));
		return 1;
	}

	while (1) {
		int i;
		fd_set read_fds;
		int maxfd, reads;
		struct timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		FD_ZERO(&read_fds);
		FD_SET(insock, &read_fds);
		maxfd = insock;

		reads = select(maxfd+1, &read_fds, NULL, NULL, &timeout);
		if (reads > 0) {
			if (FD_ISSET(insock, &read_fds)) {
				struct sockaddr_in sender_addr;
				unsigned int sender_addr_size = sizeof(sender_addr);
				int connection_id;
				char buffer[1500];
				int result = recvfrom(insock, &buffer, 1500, 0, (struct sockaddr *)&sender_addr, &sender_addr_size);
				//printf("Got %d bytes from %s:%d\n", result, inet_ntoa(sender_addr.sin_addr), htons(sender_addr.sin_port));
				if (result > 0 && ((connection_id = find_connection(&sender_addr)) >= 0 || (connection_id = add_connection(&sender_addr)) >= 0)) {
					handle_connection(&connections[connection_id], buffer, result);
				} else if (result > 0) {
					printf("Max connections exceeded! Dropping packet\n");
				} else {
					printf("Fatal socket error?!\n");
				}
			}
		}

		/* TODO: Write a portable way to check how many ms since last loop.
		 * Ex; gettimeofday() on POSIX and GetTickCount() on windows.
		 * Meanwhile, just resubmit every second.
		 */
		int16_t timestamp = time(NULL);
		for (i = MAX_OUTGOING_QUEUE; i--;) {
			struct un_outgoing_queue *entry = &outgoing_queue[i];
			if (entry->inuse && entry->timestamp < timestamp) {
				printf("Resending for %d time (ts %d != %d)\n", entry->sent, entry->timestamp, timestamp);
				replyto(entry->conn, (char *)&entry->packet, entry->packet.packetlen);
				entry->timestamp = timestamp;
				if (entry->sent++ == 4) {
					entry->inuse = 0;
					/* TODO: What to do now? Disconnect? */
					printf("Timeout sending packet %d to %s\n", MAX_OUTGOING_QUEUE - i, inet_ntoa(entry->conn->addr.sin_addr)); 
				}
			}
		}
	}

	return 0;
}

