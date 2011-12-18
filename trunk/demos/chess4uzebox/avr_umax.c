/***************************************************************************/
/*                               micro-Max,                                */
/* A chess program smaller than 2KB (of non-blank source), by H.G. Muller  */
/* Port to Atmel ATMega644 and AVR GCC, by Andre Adrian                    */
/* Several modifications for use in chess4uzebox, by Martin Šustek         */
/***************************************************************************/
/* version 4.8 (1953 characters) features:                                 */
/* - recursive negamax search                                              */
/* - all-capture MVV/LVA quiescence search                                 */
/* - (internal) iterative deepening                                        */
/* - best-move-first 'sorting'                                             */
/* - a hash table storing score and best move                              */
/* - futility pruning                                                      */
/* - king safety through magnetic, frozen king in middle-game              */
/* - R=2 null-move pruning                                                 */
/* - keep hash and repetition-draw detection                               */
/* - better defense against passers through gradual promotion              */
/* - extend check evasions in inner nodes                                  */
/* - reduction of all non-Pawn, non-capture moves except hash move (LMR)   */
/* - full FIDE rules and move-legality checking                            */

/* 25jul2001 added option to exit (interrupt) game                         */
/* 23jul2011 hack: at endgame, no one square should be yellow              */
/* 23jul2011 hack: print_log detects mate, so can break main cycle         */
/* 21jul2011 removed unix and verbose functions                            */
/* 21jul2011 added callbacks to disable UI and speed up thinking           */
/* 19jul2011 added function new_game to restart game                       */
/* 17jul2011 added ZMAX parameter to tune cpu moves quality at runtime     */
/* 10jul2011 added optional under-promotion for human player (fifth char   */
/*           is R - rook, B - bishop, N - knight)                          */
/* 10jul2011 added hooks for input / output from UI                        */
/* 26nov2008 no hash table                                                 */
/* 29nov2008 all IO via myputchar(), mygetchar(), pseudo random generator  */

#define W while
#define M 0x88
#define S 128
#define I 8000

char ZMAX;  /* variable stack limit for tuning cpu moves quality */
long N, T;                  /* N=evaluated positions+S, T=recursion limit */
short Q,O,K,R,k;        /* k=moving side */
char *p,c[7],Z;   /* p=pointer to c, c=user input, computer output, Z=recursion counter */

char L,w[8],o[32],b[129];

void new_game() {
	ZMAX = 6;
	k = 16;

	w[0]=0;/* relative piece values    */
	w[1]=2;
	w[2]=2;
	w[3]=7;
	w[4]=-1;
	w[5]=8;
	w[6]=12;
	w[7]=23;

	o[0]=-16;/* step-vector lists */
	o[1]=-15;
	o[2]=-17;
	o[3]=0;
	o[4]=1;
	o[5]=16;
	o[6]=0;
	o[7]=1;
	o[8]=16;
	o[9]=15;
	o[10]=17;
	o[11]=0;
	o[12]=14;
	o[13]=18;
	o[14]=31;
	o[15]=33;
	o[16]=0;
	o[17]=7;/* 1st dir. in o[] per piece*/
	o[18]=-1;
	o[19]=11;
	o[20]=6;
	o[21]=8;
	o[22]=3;
	o[23]=6;
	o[24]=6;/* initial piece setup      */
	o[25]=3;
	o[26]=5;
	o[27]=7;
	o[28]=4;
	o[29]=5;
	o[30]=3;
	o[31]=6;

	b[0]=22;/* board is left part, center-pts table is right part, and dummy */
	b[1]=19;
	b[2]=21;
	b[3]=23;
	b[4]=20;
	b[5]=21;
	b[6]=19;
	b[7]=22;
	b[8]=28;
	b[9]=21;
	b[10]=16;
	b[11]=13;
	b[12]=12;
	b[13]=13;
	b[14]=16;
	b[15]=21;
	b[16]=18;
	b[17]=18;
	b[18]=18;
	b[19]=18;
	b[20]=18;
	b[21]=18;
	b[22]=18;
	b[23]=18;
	b[24]=22;
	b[25]=15;
	b[26]=10;
	b[27]=7;
	b[28]=6;
	b[29]=7;
	b[30]=1;
	b[31]=15;
	b[32]=0;
	b[33]=0;
	b[34]=0;
	b[35]=0;
	b[36]=0;
	b[37]=0;
	b[38]=0;
	b[39]=0;
	b[40]=18;
	b[41]=11;
	b[42]=6;
	b[43]=3;
	b[44]=2;
	b[45]=3;
	b[46]=6;
	b[47]=11;
	b[48]=0;
	b[49]=0;
	b[50]=0;
	b[51]=0;
	b[52]=0;
	b[53]=0;
	b[54]=0;
	b[55]=0;
	b[56]=16;
	b[57]=9;
	b[58]=4;
	b[59]=1;
	b[60]=0;
	b[61]=1;
	b[62]=4;
	b[63]=9;
	b[64]=0;
	b[65]=0;
	b[66]=0;
	b[67]=0;
	b[68]=0;
	b[69]=0;
	b[70]=0;
	b[71]=0;
	b[72]=16;
	b[73]=9;
	b[74]=4;
	b[75]=1;
	b[76]=0;
	b[77]=1;
	b[78]=4;
	b[79]=9;
	b[80]=0;
	b[81]=0;
	b[82]=0;
	b[83]=0;
	b[84]=0;
	b[85]=0;
	b[86]=0;
	b[87]=0;
	b[88]=18;
	b[89]=11;
	b[90]=6;
	b[91]=3;
	b[92]=2;
	b[93]=3;
	b[94]=6;
	b[95]=11;
	b[96]=9;
	b[97]=9;
	b[98]=9;
	b[99]=9;
	b[100]=9;
	b[101]=9;
	b[102]=9;
	b[103]=9;
	b[104]=22;
	b[105]=15;
	b[106]=10;
	b[107]=7;
	b[108]=6;
	b[109]=7;
	b[110]=10;
	b[111]=15;
	b[112]=14;
	b[113]=11;
	b[114]=13;
	b[115]=15;
	b[116]=12;
	b[117]=13;
	b[118]=11;
	b[119]=14;
	b[120]=28;
	b[121]=21;
	b[122]=16;
	b[123]=13;
	b[124]=12;
	b[125]=13;
	b[126]=16;
	b[127]=21;
	b[128]=0;
/*
	for (uint8_t integ = 0; integ <= 128; integ++) {
		if ((integ == 0) || (integ && 0x88)) {
			b[integ] = 0;
		}
	}
	b[0x00] = 4+8;
	b[0x07] = 4+16;
	b[0x21] = 6+16;
	b[0x14] = 6+16;
	b[0x26] = 1+8;
	b[0x63] = 2+16;
	k = 8;
/*
	#include "elo_test.h"
	test1();
*/
}

/* 16bit pseudo random generator */
#define MYRAND_MAX 65535

unsigned short r = 1;                     /* pseudo random generator seed */

void mysrand(unsigned short r_) {
 r = r_;
}

unsigned short myrand(void) {
 return r=((r<<11)+(r<<7)+r)>>1;
}

short D(q,l,e,E,z,n)                          /* recursive minimax search */
short q,l,e;                      /* (q,l)=window, e=current eval. score, */
unsigned char E,z,n;     /* E=e.p. sqr.z=prev.dest, n=depth; return score */        
{                       
 short m,v,i,P,V,s;
 unsigned char t,p,u,x,y,X,Y,H,B,j,d,h,F,G,C;
 signed char r;

 break_joypad |= ReadJoypad(0) | ReadJoypad(1);/* change flag to break     */

 if (++Z>6) {                                  /* stack underrun check     */
  --Z;return e;                                    
 }
 
 q--;                                          /* adj. window: delay bonus */
 k^=24;                                        /* change sides             */
 d=Y=0;                                        /* start iter. from scratch */
 X=myrand()&~M;                                /* start at random field    */
 W(d++<n||d<3||                                /* iterative deepening loop */
   z&K==I&&(N<T&d<98||                         /* root: deepen upto time   */
   (K=X,L=Y&~M,d=3)))                          /* time's up: go do best    */
 {x=B=X;                                       /* start scan at prev. best */
  h=Y&S;                                       /* request try noncastl. 1st*/
  P=d<3?I:D(-l,1-l,-e,S,0,d-3);                /* Search null move         */
  m=-P<l|R>35?d>2?-I:e:-P;                     /* Prune or stand-pat       */
  ++N;                                         /* node count (for timing)  */
  do{u=b[x];                                   /* scan board looking for   */
   if(u&k)                                     /*  own piece (inefficient!)*/
   {r=p=u&7;                                   /* p = piece type (set r>0) */
    j=o[p+16];                                 /* first step vector f.piece*/
    W(r=p>2&r<0?-r:-o[++j])                    /* loop over directions o[] */
    {A:                                        /* resume normal after best */
     y=x;F=G=S;                                /* (x,y)=move, (F,G)=castl.R*/
     do{                                       /* y traverses ray, or:     */
      H=y=h?Y^h:y+r;                           /* sneak in prev. best move */
      if(y&M)break;                            /* board edge hit           */
      m=E-S&b[E]&&y-E<2&E-y<2?I:m;             /* bad castling             */
      if(p<3&y==E)H^=16;                       /* shift capt.sqr. H if e.p.*/
      t=b[H];if(t&k|p<3&!(y-x&7)-!t)break;     /* capt. own, bad pawn mode */
      i=37*w[t&7]+(t&192);                     /* value of capt. piece t   */
      m=i<0?I:m;                               /* K capture                */
      if(m>=l&d>1)goto C;                      /* abort on fail high       */
      v=d-1?e:i-p;                             /* MVV/LVA scoring          */
      if(d-!t>1)                               /* remaining depth          */
      {v=p<6?b[x+8]-b[y+8]:0;                  /* center positional pts.   */
       b[G]=b[H]=b[x]=0;b[y]=u|32;             /* do move, set non-virgin  */
       if(!(G&M))b[F]=k+6,v+=50;               /* castling: put R & score  */
       v-=p-4|R>29?0:20;                       /* penalize mid-game K move */
       if(p<3)                                 /* pawns:                   */
       {v-=9*((x-2&M||b[x-2]-u)+               /* structure, undefended    */
              (x+2&M||b[x+2]-u)-1              /*        squares plus bias */
             +(b[x^16]==k+36))                 /* kling to non-virgin King */
             -(R>>2);                          /* end-game Pawn-push bonus */
        V=y+r+1&S?647-p:2*(u&y+16&32);         /* promotion or 6/7th bonus */
        b[y]+=V;i+=V;                          /* change piece, add score  */
        switch (c[4]) {						   /* under-promotion (not so  */
        case 'N':							   /* minimalistic)            */
        	b[y]-=4;
        	break;
        case 'R':
        	b[y]-=1;
        	break;
        case 'B':
        	b[y]-=2;
        	break;
        }
       }
       v+=e+i;V=m>q?m:q;                       /* new eval and alpha       */
       C=d-1-(d>5&p>2&!t&!h);
       C=R>29|d<3|P-I?C:d;                     /* extend 1 ply if in check */
       do
        s=C>2|v>V?-D(-l,-V,-v,                 /* recursive eval. of reply */
                              F,0,C):v;        /* or fail low if futile    */
       W(s>q&++C<d);v=s;
       if(z&&K-I&&v+I&&x==K&y==L)              /* move pending & in root:  */
       {Q=-e-i;O=F;                            /*   exit if legal & found  */
        R+=i>>7;--Z;return l;                  /* captured non-P material  */
       }
       if((z==9)&&(!was_computer_move))        /* called as move-legality  */
       {if(v!=-I&x==K&y==L)                    /*   checker: if move found */
        {Q=-e-i;O=F;return l;}                 /*   & not in check, signal */
        v=m;                                   /* (prevent fail-lows on    */
       }                                       /*   K-capt. replies)       */
       b[G]=k+6;b[F]=b[y]=0;b[x]=u;b[H]=t;     /* undo move,G can be dummy */
      }
      if(v>m)                                  /* new best, update max,best*/
       m=v,X=x,Y=y|S&F;                        /* mark double move with S  */
      if(h){h=0;goto A;}                       /* redo after doing old best*/
      if(x+r-y|u&32|                           /* not 1st step,moved before*/
         p>2&(p-4|j-7||                        /* no P & no lateral K move,*/
         b[G=x+3^r>>1&7]-k-6                   /* no virgin R in corner G, */
         ||b[G^1]|b[G^2])                      /* no 2 empty sq. next to R */
        )t+=p<5;                               /* fake capt. for nonsliding*/
      else F=y;                                /* enable e.p.              */
     }W(!t);                                   /* if not capt. continue ray*/
  }}}W((x=x+9&~M)-B);                          /* next sqr. of board, wrap */
C:if(m>I-M|m<M-I)d=98;                         /* mate holds to any depth  */
  m=m+I|P==I?m:0;                              /* best loses K: (stale)mate*/
  if(z&&d>2)
   {*c='a'+(X&7);c[1]='8'-(X>>4);c[2]='a'+(Y&7);c[3]='8'-(Y>>4&7);c[4]=0;}
 }                                             /*    encoded in X S,8 bits */
 k^=24;                                        /* change sides back        */
 --Z;return m+=m<e;                            /* delayed-loss bonus       */
}

void play_game(void)
{
 break_joypad = 0;
 do{                                                 /* play loop          */
  stop_thinking();
  if (print_log()) break;                            /* in case of mate    */
  draw_board();
  read_move();
  K=I;                                               /* invalid move       */
  if(*c-10)K=*c-16*c[1]+799,L=c[2]-16*c[3]+799;      /* parse entered move */
  N=0;
  start_thinking();
  if ((break_joypad & BTN_SELECT) || (break_joypad && is_demo)) {
   exit_game = 1;
  }
 }W(!exit_game && (D(-I,I,Q,O,9,ZMAX)>-I+1));        /* think or check & do*/
 if (exit_game || is_demo) {
	 while (ReadJoypad(0) || ReadJoypad(1)) {}
	 stop_thinking();
	 return;
 }
 stop_thinking();
 was_computer_move = 1; c[0] = 'x'; c[2] = 'x';      /* don't show cursor  */
 draw_board();
}
