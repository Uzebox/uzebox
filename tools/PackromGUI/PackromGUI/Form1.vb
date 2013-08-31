Option Explicit On
Imports System.IO

Public Class Form1
    Public Structure RomHeader
        Public marker() As Byte 'Array of size 6
        Public version As Byte
        Public target As Byte
        Public progSize As UInt32
        Public year As UInt16
        Public name() As Byte 'Array of size 32 
        Public author() As Byte 'Array of size 32
        Public icon(,) As Byte 'Array (two-publicension) of size 16*16
        Public crc32 As UInt32
        Public mouse As Byte
        Public description() As Byte 'Array of size 64
    End Structure

    Public gameIcon As New Bitmap(16, 16)
    Public resizeIcon As New Bitmap(32, 32)

    Public uzeDir As String
    Public allowChanged As Boolean = False
    Public hasChanged As Boolean = False

    'Creating storage space for .uze ROM. Default
    Public rom(511) As Byte '(65536 - 4096) + 512
    Public Rhead As RomHeader

    Public crc_tab(255) As UInt32
    Public hexData(-1) As Byte
    Public uzeData(-1) As Byte

    Public Function chksum_crc32(ByRef block() As Byte, ByVal length As Integer)
        Dim crc, i As UInt32
        crc = &HFFFFFFFFUI
        For i = 0 To length - 1
            crc = ((crc >> 8) And &HFFFFFFUI) Xor (crc_tab((crc Xor block(i)) And &HFF))
        Next
        Return (crc Xor &HFFFFFFFFUI)
    End Function

    Public Sub chksum_crc32gentab()
        Dim crc, poly As UInt32
        Dim i, j As Integer
        poly = &HEDB88320UI
        For i = 0 To 255
            crc = i
            For j = 8 To 1 Step -1
                If (crc And 1) Then
                    crc = (crc >> 1) Xor poly
                Else
                    crc >>= 1
                End If
            Next
            crc_tab(i) = crc
        Next
    End Sub

    Private Sub OpenToolStripMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles OpenToolStripMenuItem.Click
        If Me.OpenFileDialog1.ShowDialog() = DialogResult.OK Then

            Dim fProp As New FileInfo(Me.OpenFileDialog1.FileName)
            Dim fStream As New FileStream(Me.OpenFileDialog1.FileName, FileMode.Open, FileAccess.Read)
            Dim bStream As New BinaryReader(fStream, System.Text.Encoding.ASCII)

            Select Case fProp.Extension.ToLower
                Case ".hex"
                    ReDim hexData(fProp.Length - 1)
                    ReDim uzeData(126976)

                    For i = 0 To (fProp.Length - 1)
                        hexData(i) = bStream.ReadByte
                    Next

                    processHex()
                    Rhead.crc32 = chksum_crc32(uzeData, Rhead.progSize)
                    fillOptions()

                    uzeDir = Nothing
                    Me.grpChip.Enabled = True
                    Me.grpDevice.Enabled = True
                    Me.grpInfo.Enabled = True
                    Me.grpInput.Enabled = True
                    Me.SaveToolStripMenuItem.Enabled = True
                    Me.SaveAsToolStripMenuItem1.Enabled = True

                    Me.Text = "Packrom - Untitled.uze*"
                Case ".uze"
                    If fProp.Length < 512 Then
                        MessageBox.Show("File is corrupt or header is missing")
                    Else
                        ReDim uzeData(fProp.Length - 513)
                        For i = 0 To 511
                            rom(i) = bStream.ReadByte
                        Next
                        For i = 0 To (fProp.Length - 513)
                            uzeData(i) = bStream.ReadByte
                        Next

                        processHeader()
                        fillOptions()

                        Me.grpChip.Enabled = True
                        Me.grpDevice.Enabled = True
                        Me.grpInfo.Enabled = True
                        Me.grpInput.Enabled = True
                        Me.SaveToolStripMenuItem.Enabled = True
                        Me.SaveAsToolStripMenuItem1.Enabled = True

                        uzeDir = fProp.FullName
                        Me.Text = "Packrom - " & fProp.Name
                        Me.allowChanged = True

                    End If
            End Select

            bStream.Close()
        End If
    End Sub

    Public Sub processHex()
        Dim bytes As Byte
        Dim address As UInt16
        Dim recordType As Byte
        Dim i As UInt32 = 0
        While (i < hexData.Length)
            If hexData(i) = Asc(":") Then
                bytes = parse_hex_byte(parse_hex_nibble(hexData(i + 1)), parse_hex_nibble(hexData(i + 2)))
                address = parse_hex_word(parse_hex_byte(parse_hex_nibble(hexData(i + 3)), parse_hex_nibble(hexData(i + 4))), parse_hex_byte(parse_hex_nibble(hexData(i + 5)), parse_hex_nibble(hexData(i + 6))))
                recordType = parse_hex_byte(parse_hex_nibble(hexData(i + 7)), parse_hex_nibble(hexData(i + 8)))

                If recordType = 0 Then
                    For q = 0 To bytes
                        uzeData(address) = parse_hex_byte(parse_hex_nibble(hexData(i + 9)), parse_hex_nibble(hexData(i + 10)))
                        address = address + 1
                        Rhead.progSize = address - 1
                        i = i + 2
                    Next
                ElseIf recordType = 1 Then
                    Exit While
                End If
            End If
            i = i + 1
        End While
        ReDim Preserve uzeData(Rhead.progSize - 1)
        If address > 61440 Then
            Rhead.target = 1
        End If
    End Sub

    Public Function parse_hex_nibble(ByVal nib As Byte) As Byte
        Dim temp As Byte
        If (nib >= Asc("0")) And (nib <= Asc("9")) Then
            temp = nib - Asc("0")
            Return temp
        ElseIf (nib >= Asc("A")) And (nib <= Asc("F")) Then
            temp = nib - Asc("A") + 10
            Return temp
        ElseIf (nib >= Asc("a")) And (nib <= Asc("f")) Then
            temp = nib - Asc("a") + 10
            Return temp
        Else
            Return 255
        End If
    End Function

    Public Function parse_hex_byte(ByVal hiNib As Byte, ByVal loNib As Byte) As Byte
        Dim temp As Byte
        temp = (hiNib << 4) Or loNib
        Return temp
    End Function

    Public Function parse_hex_word(ByVal hiByt As Byte, ByVal loByt As Byte) As UInt16
        Dim temp As UInt16
        temp = hiByt
        temp = temp << 8
        Return temp Or (loByt)
    End Function

    Public Sub processHeader()
        For i = 0 To 5
            Rhead.marker(i) = rom(i)
        Next

        Rhead.version = rom(6)
        Rhead.target = rom(7)

        Rhead.progSize = rom(11)
        Rhead.progSize = (Rhead.progSize << 8) Or rom(10)
        Rhead.progSize = (Rhead.progSize << 8) Or rom(9)
        Rhead.progSize = (Rhead.progSize << 8) Or rom(8)

        Rhead.year = rom(13)
        Rhead.year = (Rhead.year << 8) Or rom(12)

        Rhead.crc32 = rom(334)
        Rhead.crc32 = (Rhead.crc32 << 8) Or rom(335)
        Rhead.crc32 = (Rhead.crc32 << 8) Or rom(336)
        Rhead.crc32 = (Rhead.crc32 << 8) Or rom(337)

        Rhead.mouse = rom(338)

        For i = 0 To 31
            Rhead.name(i) = rom(i + 14)
        Next

        For i = 0 To 31
            Rhead.author(i) = rom(i + 46)
        Next

        For i = 0 To 63
            Rhead.description(i) = rom(i + 339)
        Next

        For q = 0 To 15
            For i = 0 To 15
                Rhead.icon(i, q) = rom(((q << 4) Or i) + 78)
            Next
        Next
    End Sub

    Public Sub fillOptions()
        Me.txtName.Text = Nothing
        For i = 0 To 31
            Me.txtName.Text = Me.txtName.Text & Chr(Rhead.name(i))
        Next

        Me.txtAuthor.Text = Nothing
        For i = 0 To 31
            Me.txtAuthor.Text = Me.txtAuthor.Text & Chr(Rhead.author(i))
        Next

        Me.txtDesc.Text = Nothing
        For i = 0 To 63
            Me.txtDesc.Text = Me.txtDesc.Text & Chr(Rhead.description(i))
        Next

        Me.txtYear.Text = Rhead.year
        Me.mtbChksum.Text = Hex(Rhead.crc32)
        Me.mtbSize.Text = Rhead.progSize

        Select Case Rhead.target
            Case 0
                Me.rdb644.Checked = True
                Me.rdb1284.Checked = False
            Case 1
                Me.rdb644.Checked = False
                Me.rdb1284.Checked = True
        End Select

        Select Case Rhead.mouse
            Case 0
                Me.rdbJoy.Checked = True
                Me.rdbMouse.Checked = False
            Case 1
                Me.rdbJoy.Checked = False
                Me.rdbMouse.Checked = True
        End Select

        For q = 0 To 15
            For i = 0 To 15
                gameIcon.SetPixel(i, q, Color.FromArgb((Rhead.icon(i, q) And 7) << 4, (Rhead.icon(i, q) And 56) << 2, (Rhead.icon(i, q) And 192)))
                'gameIcon.SetPixel(i, q, Color.FromArgb(Rhead.icon(i, q) And 7, (Rhead.icon(i, q) And 56) >> 3, (Rhead.icon(i, q) And 192) >> 6))
            Next
        Next

        For q = 0 To 31
            For i = 0 To 31
                resizeIcon.SetPixel(i, q, gameIcon.GetPixel((i Xor (i And 1)) >> 1, (q Xor (q And 1)) >> 1))
            Next
        Next

        Me.pbIcon.Image = resizeIcon
    End Sub

    Private Sub SaveToolStripMenuItem_Click(sender As System.Object, e As System.EventArgs) Handles SaveToolStripMenuItem.Click
        If Not File.Exists(uzeDir) Then
            Me.SaveFileDialog1.FileName = "Untitled"
            'Me.SaveFileDialog1.ShowDialog()
            'File.Create(uzeDir).Close()
            UzeToolStripMenuItem_Click(SaveToolStripMenuItem, System.EventArgs.Empty)
        ElseIf File.Exists(uzeDir) Then
            Dim fProp As New FileInfo(uzeDir)
            Dim fStream As New FileStream(uzeDir, FileMode.Create, FileAccess.Write)
            Dim bStream As New BinaryWriter(fStream, System.Text.Encoding.UTF8)

            storeOptions()
            moveOptions()
            bStream.Write(rom)
            bStream.Close()
            Me.Text = "Packrom - " & fProp.Name
            hasChanged = False
        End If
    End Sub

    Private Sub UzeToolStripMenuItem_Click(sender As System.Object, e As System.EventArgs) Handles UzeToolStripMenuItem.Click
        Me.SaveFileDialog1.ShowDialog()
        If Me.SaveFileDialog1.FileName <> Nothing Then
            Dim fProp As New FileInfo(Me.SaveFileDialog1.FileName)
            Dim fStream As New FileStream(Me.SaveFileDialog1.FileName, FileMode.Create, FileAccess.Write)
            Dim bStream As New BinaryWriter(fStream, System.Text.Encoding.UTF8)

            uzeDir = Me.SaveFileDialog1.FileName
            storeOptions()
            moveOptions()
            bStream.Write(rom)
            bStream.Close()
            Me.Text = "Packrom - " & fProp.Name
            hasChanged = False
        End If
    End Sub

    Private Sub Form1_Load(sender As System.Object, e As System.EventArgs) Handles MyBase.Load
        ReDim Rhead.marker(5)
        ReDim Rhead.name(31)
        ReDim Rhead.author(31)
        ReDim Rhead.icon(15, 15)
        ReDim Rhead.description(63)
        chksum_crc32gentab()
    End Sub

    Public Sub storeOptions()
        For i = 0 To Me.txtName.TextLength - 1
            Rhead.name(i) = Asc(Me.txtName.Text.Substring(i, 1))
        Next
        For i = Me.txtName.TextLength To 31
            Rhead.name(i) = Nothing
        Next

        For i = 0 To Me.txtAuthor.TextLength - 1
            Rhead.author(i) = Asc(Me.txtAuthor.Text.Substring(i, 1))
        Next
        For i = Me.txtAuthor.TextLength To 31
            Rhead.author(i) = Nothing
        Next

        For i = 0 To Me.txtDesc.TextLength - 1
            Rhead.description(i) = Asc(Me.txtDesc.Text.Substring(i, 1))
        Next
        For i = Me.txtDesc.TextLength To 63
            Rhead.description(i) = Nothing
        Next

        For q = 0 To 15
            For i = 0 To 15
                'Use conversion table in graphics editor

                Rhead.icon(i, q) = Me.gameIcon.GetPixel(i, q).ToArgb
            Next
        Next

        Rhead.year = Val(Me.txtYear.Text)
        Rhead.target = (Me.rdb644.Checked Xor 1) And Me.rdb1284.Checked
        Rhead.mouse = (Me.rdbJoy.Checked Xor 1) And Me.rdbMouse.Checked

    End Sub

    Public Sub moveOptions()
        ReDim Preserve rom(511 + Rhead.progSize)

        For i = 0 To 31
            rom(i + 14) = Rhead.name(i)
            rom(i + 46) = Rhead.author(i)
        Next
        For i = 0 To 63
            rom(i + 339) = Rhead.description(i)
        Next
        For q = 0 To 15
            For i = 0 To 15
                rom(((q << 4) Or i) + 78) = Rhead.icon(i, q)
            Next
        Next

        rom(338) = Rhead.mouse
        rom(7) = Rhead.target
        rom(12) = Rhead.year And 255
        rom(13) = (Rhead.year And 65280) >> 8

        rom(8) = Rhead.progSize And 255
        rom(9) = (Rhead.progSize And 65280) >> 8
        rom(10) = (Rhead.progSize And 16711680) >> 16
        rom(11) = (Rhead.progSize And 4278190080) >> 24

        rom(334) = Rhead.crc32 And 255
        rom(335) = (Rhead.crc32 And 65280) >> 8
        rom(336) = (Rhead.crc32 And 16711680) >> 16
        rom(337) = (Rhead.crc32 And 4278190080) >> 24

        For i = 0 To Rhead.progSize - 1
            rom(512 + i) = uzeData(i)
        Next
    End Sub

    Private Sub PictureBox1_Click(sender As System.Object, e As System.EventArgs) Handles pbIcon.Click
        If Dialog1.ShowDialog() = Windows.Forms.DialogResult.OK Then
            For q = 0 To 31
                For i = 0 To 31
                    resizeIcon.SetPixel(i, q, gameIcon.GetPixel((i Xor (i And 1)) >> 1, (q Xor (q And 1)) >> 1))
                Next
            Next

            Me.pbIcon.Image = resizeIcon
        End If
    End Sub

    Private Sub AboutToolStripMenuItem_Click(sender As System.Object, e As System.EventArgs) Handles AboutToolStripMenuItem.Click
        AboutBox1.ShowDialog()
    End Sub

    Private Sub txtName_TextChanged(sender As System.Object, e As System.EventArgs) Handles txtName.TextChanged, txtAuthor.TextChanged, txtDesc.TextChanged, txtYear.TextChanged, rdb1284.CheckedChanged, rdb644.CheckedChanged, rdbCon.CheckedChanged, rdbJam.CheckedChanged, rdbJoy.CheckedChanged, rdbMouse.CheckedChanged
        If (Me.allowChanged = True) And (Me.hasChanged = False) Then
            Me.Text = Me.Text + "*"
            Me.hasChanged = True
        End If
    End Sub

    Private Sub ExitToolStripMenuItem_Click(sender As System.Object, e As System.EventArgs) Handles ExitToolStripMenuItem.Click
        Me.Close()
    End Sub

    Private Sub txtYear_KeyPress(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyPressEventArgs) Handles txtYear.KeyPress
        If (Asc(e.KeyChar) < 48) Or (Asc(e.KeyChar) > 57) Then
            e.Handled = True
        End If
        If (Asc(e.KeyChar) = 8) Then 'Backspace
            e.Handled = False
        End If
    End Sub

    Private Function rgTab(ByRef 
End Class

