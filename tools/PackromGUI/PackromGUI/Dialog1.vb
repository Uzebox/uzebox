Imports System.Windows.Forms

Public Class Dialog1
    Dim editIcon As New Bitmap(128, 128)
    Dim currentColor, currentR, currentG, currentB As Byte
    Dim rgTable() = {0, 51, 87, 103, 128, 160, 207, 255}
    'Dim rgTable() = {0, 51, 87, 103, 127, 155, 207, 255}
    Dim bTable() = {0, 103, 175, 255}
    Private Sub OK_Button_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles OK_Button.Click
        Me.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.Close()
    End Sub

    Private Sub Cancel_Button_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Cancel_Button.Click
        Me.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.Close()

    End Sub

    Private Sub PictureBox1_MouseDown(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles PictureBox1.MouseDown
        Form1.gameIcon.SetPixel((e.X Xor (e.X And 3)) >> 3, (e.Y Xor (e.Y And 3)) >> 3, Color.FromArgb(currentR, currentG, currentB))
        reloadIcon()
    End Sub

    Private Sub PictureBox1_MouseMove(sender As Object, e As System.Windows.Forms.MouseEventArgs) Handles PictureBox1.MouseMove
        If (e.X < 128) And (e.Y < 128) And (e.X > -1) And (e.Y > -1) Then
            Me.lblHovercolor.Text = Bin(Form1.Rhead.icon((e.X Xor (e.X And 3)) >> 3, (e.Y Xor (e.Y And 3)) >> 3))
            Me.lblHovercolorD.Text = Form1.Rhead.icon((e.X Xor (e.X And 3)) >> 3, (e.Y Xor (e.Y And 3)) >> 3)
            Me.lblXcoord.Text = (e.X Xor (e.X And 3)) >> 3
            Me.lblYcoord.Text = (e.Y Xor (e.Y And 3)) >> 3
        End If
    End Sub

    Private Sub Dialog1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        reloadIcon()
    End Sub
    Private Sub reloadIcon()
        For q = 0 To 127
            For i = 0 To 127
                Me.editIcon.SetPixel(i, q, Form1.gameIcon.GetPixel((i Xor (i And 3)) >> 3, (q Xor (q And 3)) >> 3))
            Next
        Next
        Me.PictureBox1.Image = editIcon
    End Sub

    Private Sub trkRed_Scroll(sender As System.Object, e As System.EventArgs) Handles trkRed.Scroll, trkGreen.Scroll, trkBlue.Scroll
        Dim currentSlide As New TrackBar
        currentSlide = sender
        Select Case currentSlide.Name
            Case "trkRed"
                currentColor = (currentColor And 248) Or currentSlide.Value
                Me.lblRed.Text = currentSlide.Value
                currentR = rgTable((currentColor And 7))
            Case "trkGreen"
                currentColor = (currentColor And 199) Or (currentSlide.Value << 3)
                Me.lblGreen.Text = currentSlide.Value
                currentG = rgTable((currentColor And 56) >> 3)
            Case "trkBlue"
                currentColor = (currentColor And 63) Or (currentSlide.Value << 6)
                Me.lblBlue.Text = currentSlide.Value
                currentB = bTable((currentColor And 192) >> 6)
        End Select
        Me.btnColor.BackColor = Color.FromArgb(rgTable((currentColor And 7)), rgTable((currentColor And 56) >> 3), bTable((currentColor And 192) >> 6))
        'Me.btnColor.BackColor = Color.FromArgb((currentColor And 7) << 5, (currentColor And 56) << 2, (currentColor And 192))
        Me.lblColor.Text = Bin(currentColor)
        Me.lblColorD.Text = currentColor
    End Sub

    Private Function Bin(ByVal input As Byte) As String
        Dim temp As String = Nothing
        For i = 7 To 0 Step -1
            temp = temp & ((input And (2 ^ i)) >> i)
        Next
        Return temp
    End Function
End Class
