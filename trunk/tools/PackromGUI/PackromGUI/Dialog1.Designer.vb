<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Dialog1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.TableLayoutPanel1 = New System.Windows.Forms.TableLayoutPanel()
        Me.OK_Button = New System.Windows.Forms.Button()
        Me.Cancel_Button = New System.Windows.Forms.Button()
        Me.PictureBox1 = New System.Windows.Forms.PictureBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.btnColor = New System.Windows.Forms.Button()
        Me.Label12 = New System.Windows.Forms.Label()
        Me.Label13 = New System.Windows.Forms.Label()
        Me.lblXcoord = New System.Windows.Forms.Label()
        Me.lblYcoord = New System.Windows.Forms.Label()
        Me.lblColor = New System.Windows.Forms.Label()
        Me.trkBlue = New System.Windows.Forms.TrackBar()
        Me.trkGreen = New System.Windows.Forms.TrackBar()
        Me.trkRed = New System.Windows.Forms.TrackBar()
        Me.lblRed = New System.Windows.Forms.Label()
        Me.lblBlue = New System.Windows.Forms.Label()
        Me.lblGreen = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.lblHovercolor = New System.Windows.Forms.Label()
        Me.lblColorD = New System.Windows.Forms.Label()
        Me.lblHovercolorD = New System.Windows.Forms.Label()
        Me.TableLayoutPanel1.SuspendLayout()
        CType(Me.PictureBox1, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trkBlue, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trkGreen, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trkRed, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'TableLayoutPanel1
        '
        Me.TableLayoutPanel1.Anchor = CType((System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.TableLayoutPanel1.ColumnCount = 2
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.ColumnStyles.Add(New System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.Controls.Add(Me.OK_Button, 0, 0)
        Me.TableLayoutPanel1.Controls.Add(Me.Cancel_Button, 1, 0)
        Me.TableLayoutPanel1.Location = New System.Drawing.Point(167, 239)
        Me.TableLayoutPanel1.Name = "TableLayoutPanel1"
        Me.TableLayoutPanel1.RowCount = 1
        Me.TableLayoutPanel1.RowStyles.Add(New System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50.0!))
        Me.TableLayoutPanel1.Size = New System.Drawing.Size(146, 29)
        Me.TableLayoutPanel1.TabIndex = 0
        '
        'OK_Button
        '
        Me.OK_Button.Anchor = System.Windows.Forms.AnchorStyles.None
        Me.OK_Button.Location = New System.Drawing.Point(3, 3)
        Me.OK_Button.Name = "OK_Button"
        Me.OK_Button.Size = New System.Drawing.Size(67, 23)
        Me.OK_Button.TabIndex = 0
        Me.OK_Button.Text = "OK"
        '
        'Cancel_Button
        '
        Me.Cancel_Button.Anchor = System.Windows.Forms.AnchorStyles.None
        Me.Cancel_Button.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.Cancel_Button.Location = New System.Drawing.Point(76, 3)
        Me.Cancel_Button.Name = "Cancel_Button"
        Me.Cancel_Button.Size = New System.Drawing.Size(67, 23)
        Me.Cancel_Button.TabIndex = 1
        Me.Cancel_Button.Text = "Cancel"
        '
        'PictureBox1
        '
        Me.PictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.PictureBox1.ErrorImage = Nothing
        Me.PictureBox1.InitialImage = Nothing
        Me.PictureBox1.Location = New System.Drawing.Point(179, 20)
        Me.PictureBox1.Name = "PictureBox1"
        Me.PictureBox1.Size = New System.Drawing.Size(130, 130)
        Me.PictureBox1.TabIndex = 1
        Me.PictureBox1.TabStop = False
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(124, 95)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(27, 13)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "Red"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(22, 95)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(28, 13)
        Me.Label2.TabIndex = 3
        Me.Label2.Text = "Blue"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(68, 95)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(36, 13)
        Me.Label3.TabIndex = 4
        Me.Label3.Text = "Green"
        '
        'btnColor
        '
        Me.btnColor.BackColor = System.Drawing.Color.Black
        Me.btnColor.Location = New System.Drawing.Point(47, 17)
        Me.btnColor.Name = "btnColor"
        Me.btnColor.Size = New System.Drawing.Size(75, 23)
        Me.btnColor.TabIndex = 13
        Me.btnColor.UseVisualStyleBackColor = False
        '
        'Label12
        '
        Me.Label12.AutoSize = True
        Me.Label12.Location = New System.Drawing.Point(179, 153)
        Me.Label12.Name = "Label12"
        Me.Label12.Size = New System.Drawing.Size(17, 13)
        Me.Label12.TabIndex = 14
        Me.Label12.Text = "X:"
        '
        'Label13
        '
        Me.Label13.AutoSize = True
        Me.Label13.Location = New System.Drawing.Point(179, 166)
        Me.Label13.Name = "Label13"
        Me.Label13.Size = New System.Drawing.Size(17, 13)
        Me.Label13.TabIndex = 15
        Me.Label13.Text = "Y:"
        '
        'lblXcoord
        '
        Me.lblXcoord.AutoSize = True
        Me.lblXcoord.Location = New System.Drawing.Point(198, 153)
        Me.lblXcoord.Name = "lblXcoord"
        Me.lblXcoord.Size = New System.Drawing.Size(0, 13)
        Me.lblXcoord.TabIndex = 16
        '
        'lblYcoord
        '
        Me.lblYcoord.AutoSize = True
        Me.lblYcoord.Location = New System.Drawing.Point(198, 166)
        Me.lblYcoord.Name = "lblYcoord"
        Me.lblYcoord.Size = New System.Drawing.Size(0, 13)
        Me.lblYcoord.TabIndex = 17
        '
        'lblColor
        '
        Me.lblColor.AutoSize = True
        Me.lblColor.Location = New System.Drawing.Point(56, 43)
        Me.lblColor.Name = "lblColor"
        Me.lblColor.Size = New System.Drawing.Size(55, 13)
        Me.lblColor.TabIndex = 18
        Me.lblColor.Text = "00000000"
        '
        'trkBlue
        '
        Me.trkBlue.LargeChange = 1
        Me.trkBlue.Location = New System.Drawing.Point(15, 110)
        Me.trkBlue.Maximum = 3
        Me.trkBlue.Name = "trkBlue"
        Me.trkBlue.Orientation = System.Windows.Forms.Orientation.Vertical
        Me.trkBlue.Size = New System.Drawing.Size(45, 104)
        Me.trkBlue.TabIndex = 19
        Me.trkBlue.TickStyle = System.Windows.Forms.TickStyle.Both
        '
        'trkGreen
        '
        Me.trkGreen.LargeChange = 1
        Me.trkGreen.Location = New System.Drawing.Point(66, 110)
        Me.trkGreen.Maximum = 7
        Me.trkGreen.Name = "trkGreen"
        Me.trkGreen.Orientation = System.Windows.Forms.Orientation.Vertical
        Me.trkGreen.Size = New System.Drawing.Size(45, 104)
        Me.trkGreen.TabIndex = 20
        Me.trkGreen.TickStyle = System.Windows.Forms.TickStyle.Both
        '
        'trkRed
        '
        Me.trkRed.LargeChange = 1
        Me.trkRed.Location = New System.Drawing.Point(117, 110)
        Me.trkRed.Maximum = 7
        Me.trkRed.Name = "trkRed"
        Me.trkRed.Orientation = System.Windows.Forms.Orientation.Vertical
        Me.trkRed.Size = New System.Drawing.Size(45, 104)
        Me.trkRed.TabIndex = 21
        Me.trkRed.TickStyle = System.Windows.Forms.TickStyle.Both
        '
        'lblRed
        '
        Me.lblRed.AutoSize = True
        Me.lblRed.Location = New System.Drawing.Point(124, 218)
        Me.lblRed.Name = "lblRed"
        Me.lblRed.Size = New System.Drawing.Size(13, 13)
        Me.lblRed.TabIndex = 24
        Me.lblRed.Text = "0"
        '
        'lblBlue
        '
        Me.lblBlue.AutoSize = True
        Me.lblBlue.Location = New System.Drawing.Point(12, 218)
        Me.lblBlue.Name = "lblBlue"
        Me.lblBlue.Size = New System.Drawing.Size(13, 13)
        Me.lblBlue.TabIndex = 25
        Me.lblBlue.Text = "0"
        '
        'lblGreen
        '
        Me.lblGreen.AutoSize = True
        Me.lblGreen.Location = New System.Drawing.Point(68, 218)
        Me.lblGreen.Name = "lblGreen"
        Me.lblGreen.Size = New System.Drawing.Size(13, 13)
        Me.lblGreen.TabIndex = 26
        Me.lblGreen.Text = "0"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(179, 180)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(34, 13)
        Me.Label7.TabIndex = 27
        Me.Label7.Text = "Color:"
        '
        'lblHovercolor
        '
        Me.lblHovercolor.AutoSize = True
        Me.lblHovercolor.Location = New System.Drawing.Point(219, 180)
        Me.lblHovercolor.Name = "lblHovercolor"
        Me.lblHovercolor.Size = New System.Drawing.Size(0, 13)
        Me.lblHovercolor.TabIndex = 28
        '
        'lblColorD
        '
        Me.lblColorD.AutoSize = True
        Me.lblColorD.Location = New System.Drawing.Point(68, 65)
        Me.lblColorD.Name = "lblColorD"
        Me.lblColorD.Size = New System.Drawing.Size(13, 13)
        Me.lblColorD.TabIndex = 29
        Me.lblColorD.Text = "0"
        '
        'lblHovercolorD
        '
        Me.lblHovercolorD.AutoSize = True
        Me.lblHovercolorD.Location = New System.Drawing.Point(219, 193)
        Me.lblHovercolorD.Name = "lblHovercolorD"
        Me.lblHovercolorD.Size = New System.Drawing.Size(0, 13)
        Me.lblHovercolorD.TabIndex = 30
        '
        'Dialog1
        '
        Me.AcceptButton = Me.OK_Button
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.CancelButton = Me.Cancel_Button
        Me.ClientSize = New System.Drawing.Size(325, 280)
        Me.Controls.Add(Me.lblHovercolorD)
        Me.Controls.Add(Me.lblColorD)
        Me.Controls.Add(Me.lblHovercolor)
        Me.Controls.Add(Me.Label7)
        Me.Controls.Add(Me.lblGreen)
        Me.Controls.Add(Me.lblBlue)
        Me.Controls.Add(Me.lblRed)
        Me.Controls.Add(Me.trkRed)
        Me.Controls.Add(Me.trkGreen)
        Me.Controls.Add(Me.trkBlue)
        Me.Controls.Add(Me.lblColor)
        Me.Controls.Add(Me.lblYcoord)
        Me.Controls.Add(Me.lblXcoord)
        Me.Controls.Add(Me.Label13)
        Me.Controls.Add(Me.Label12)
        Me.Controls.Add(Me.btnColor)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.PictureBox1)
        Me.Controls.Add(Me.TableLayoutPanel1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "Dialog1"
        Me.ShowInTaskbar = False
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
        Me.Text = "Icon"
        Me.TableLayoutPanel1.ResumeLayout(False)
        CType(Me.PictureBox1, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trkBlue, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trkGreen, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trkRed, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents TableLayoutPanel1 As System.Windows.Forms.TableLayoutPanel
    Friend WithEvents OK_Button As System.Windows.Forms.Button
    Friend WithEvents Cancel_Button As System.Windows.Forms.Button
    Friend WithEvents PictureBox1 As System.Windows.Forms.PictureBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents btnColor As System.Windows.Forms.Button
    Friend WithEvents Label12 As System.Windows.Forms.Label
    Friend WithEvents Label13 As System.Windows.Forms.Label
    Friend WithEvents lblXcoord As System.Windows.Forms.Label
    Friend WithEvents lblYcoord As System.Windows.Forms.Label
    Friend WithEvents lblColor As System.Windows.Forms.Label
    Friend WithEvents trkBlue As System.Windows.Forms.TrackBar
    Friend WithEvents trkGreen As System.Windows.Forms.TrackBar
    Friend WithEvents trkRed As System.Windows.Forms.TrackBar
    Friend WithEvents lblRed As System.Windows.Forms.Label
    Friend WithEvents lblBlue As System.Windows.Forms.Label
    Friend WithEvents lblGreen As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents lblHovercolor As System.Windows.Forms.Label
    Friend WithEvents lblColorD As System.Windows.Forms.Label
    Friend WithEvents lblHovercolorD As System.Windows.Forms.Label

End Class
