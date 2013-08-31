<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
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
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(Form1))
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.OpenFileDialog1 = New System.Windows.Forms.OpenFileDialog()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.MenuStrip1 = New System.Windows.Forms.MenuStrip()
        Me.FileToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.OpenToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.SaveToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.SaveAsToolStripMenuItem1 = New System.Windows.Forms.ToolStripMenuItem()
        Me.UzeToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.HexToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.ExitToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.HelpToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.AboutToolStripMenuItem = New System.Windows.Forms.ToolStripMenuItem()
        Me.grpChip = New System.Windows.Forms.GroupBox()
        Me.rdb1284 = New System.Windows.Forms.RadioButton()
        Me.rdb644 = New System.Windows.Forms.RadioButton()
        Me.grpDevice = New System.Windows.Forms.GroupBox()
        Me.rdbJam = New System.Windows.Forms.RadioButton()
        Me.rdbCon = New System.Windows.Forms.RadioButton()
        Me.grpInfo = New System.Windows.Forms.GroupBox()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.pbIcon = New System.Windows.Forms.PictureBox()
        Me.mtbSize = New System.Windows.Forms.MaskedTextBox()
        Me.txtDesc = New System.Windows.Forms.TextBox()
        Me.txtAuthor = New System.Windows.Forms.TextBox()
        Me.txtYear = New System.Windows.Forms.TextBox()
        Me.txtName = New System.Windows.Forms.TextBox()
        Me.label6 = New System.Windows.Forms.Label()
        Me.mtbChksum = New System.Windows.Forms.MaskedTextBox()
        Me.SaveFileDialog1 = New System.Windows.Forms.SaveFileDialog()
        Me.grpInput = New System.Windows.Forms.GroupBox()
        Me.rdbMouse = New System.Windows.Forms.RadioButton()
        Me.rdbJoy = New System.Windows.Forms.RadioButton()
        Me.MenuStrip1.SuspendLayout()
        Me.grpChip.SuspendLayout()
        Me.grpDevice.SuspendLayout()
        Me.grpInfo.SuspendLayout()
        CType(Me.pbIcon, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.grpInput.SuspendLayout()
        Me.SuspendLayout()
        '
        'Label1
        '
        resources.ApplyResources(Me.Label1, "Label1")
        Me.Label1.Name = "Label1"
        '
        'Label2
        '
        resources.ApplyResources(Me.Label2, "Label2")
        Me.Label2.Name = "Label2"
        '
        'Label3
        '
        resources.ApplyResources(Me.Label3, "Label3")
        Me.Label3.Name = "Label3"
        '
        'Label4
        '
        resources.ApplyResources(Me.Label4, "Label4")
        Me.Label4.Name = "Label4"
        '
        'OpenFileDialog1
        '
        resources.ApplyResources(Me.OpenFileDialog1, "OpenFileDialog1")
        '
        'Label5
        '
        resources.ApplyResources(Me.Label5, "Label5")
        Me.Label5.Name = "Label5"
        '
        'MenuStrip1
        '
        Me.MenuStrip1.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.FileToolStripMenuItem, Me.HelpToolStripMenuItem})
        resources.ApplyResources(Me.MenuStrip1, "MenuStrip1")
        Me.MenuStrip1.Name = "MenuStrip1"
        '
        'FileToolStripMenuItem
        '
        Me.FileToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.OpenToolStripMenuItem, Me.SaveToolStripMenuItem, Me.SaveAsToolStripMenuItem1, Me.ExitToolStripMenuItem})
        Me.FileToolStripMenuItem.Name = "FileToolStripMenuItem"
        resources.ApplyResources(Me.FileToolStripMenuItem, "FileToolStripMenuItem")
        '
        'OpenToolStripMenuItem
        '
        Me.OpenToolStripMenuItem.Name = "OpenToolStripMenuItem"
        resources.ApplyResources(Me.OpenToolStripMenuItem, "OpenToolStripMenuItem")
        '
        'SaveToolStripMenuItem
        '
        resources.ApplyResources(Me.SaveToolStripMenuItem, "SaveToolStripMenuItem")
        Me.SaveToolStripMenuItem.Name = "SaveToolStripMenuItem"
        '
        'SaveAsToolStripMenuItem1
        '
        Me.SaveAsToolStripMenuItem1.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.UzeToolStripMenuItem, Me.HexToolStripMenuItem})
        resources.ApplyResources(Me.SaveAsToolStripMenuItem1, "SaveAsToolStripMenuItem1")
        Me.SaveAsToolStripMenuItem1.Name = "SaveAsToolStripMenuItem1"
        '
        'UzeToolStripMenuItem
        '
        Me.UzeToolStripMenuItem.Name = "UzeToolStripMenuItem"
        resources.ApplyResources(Me.UzeToolStripMenuItem, "UzeToolStripMenuItem")
        '
        'HexToolStripMenuItem
        '
        Me.HexToolStripMenuItem.Name = "HexToolStripMenuItem"
        resources.ApplyResources(Me.HexToolStripMenuItem, "HexToolStripMenuItem")
        '
        'ExitToolStripMenuItem
        '
        Me.ExitToolStripMenuItem.Name = "ExitToolStripMenuItem"
        resources.ApplyResources(Me.ExitToolStripMenuItem, "ExitToolStripMenuItem")
        '
        'HelpToolStripMenuItem
        '
        Me.HelpToolStripMenuItem.DropDownItems.AddRange(New System.Windows.Forms.ToolStripItem() {Me.AboutToolStripMenuItem})
        Me.HelpToolStripMenuItem.Name = "HelpToolStripMenuItem"
        resources.ApplyResources(Me.HelpToolStripMenuItem, "HelpToolStripMenuItem")
        '
        'AboutToolStripMenuItem
        '
        Me.AboutToolStripMenuItem.Name = "AboutToolStripMenuItem"
        resources.ApplyResources(Me.AboutToolStripMenuItem, "AboutToolStripMenuItem")
        '
        'grpChip
        '
        Me.grpChip.Controls.Add(Me.rdb1284)
        Me.grpChip.Controls.Add(Me.rdb644)
        resources.ApplyResources(Me.grpChip, "grpChip")
        Me.grpChip.Name = "grpChip"
        Me.grpChip.TabStop = False
        '
        'rdb1284
        '
        resources.ApplyResources(Me.rdb1284, "rdb1284")
        Me.rdb1284.Name = "rdb1284"
        Me.rdb1284.UseVisualStyleBackColor = True
        '
        'rdb644
        '
        resources.ApplyResources(Me.rdb644, "rdb644")
        Me.rdb644.Name = "rdb644"
        Me.rdb644.UseVisualStyleBackColor = True
        '
        'grpDevice
        '
        Me.grpDevice.Controls.Add(Me.rdbJam)
        Me.grpDevice.Controls.Add(Me.rdbCon)
        resources.ApplyResources(Me.grpDevice, "grpDevice")
        Me.grpDevice.Name = "grpDevice"
        Me.grpDevice.TabStop = False
        '
        'rdbJam
        '
        resources.ApplyResources(Me.rdbJam, "rdbJam")
        Me.rdbJam.Name = "rdbJam"
        Me.rdbJam.UseVisualStyleBackColor = True
        '
        'rdbCon
        '
        resources.ApplyResources(Me.rdbCon, "rdbCon")
        Me.rdbCon.Name = "rdbCon"
        Me.rdbCon.UseVisualStyleBackColor = True
        '
        'grpInfo
        '
        Me.grpInfo.Controls.Add(Me.Label8)
        Me.grpInfo.Controls.Add(Me.Label7)
        Me.grpInfo.Controls.Add(Me.pbIcon)
        Me.grpInfo.Controls.Add(Me.mtbSize)
        Me.grpInfo.Controls.Add(Me.txtDesc)
        Me.grpInfo.Controls.Add(Me.txtAuthor)
        Me.grpInfo.Controls.Add(Me.txtYear)
        Me.grpInfo.Controls.Add(Me.txtName)
        Me.grpInfo.Controls.Add(Me.label6)
        Me.grpInfo.Controls.Add(Me.mtbChksum)
        Me.grpInfo.Controls.Add(Me.Label1)
        Me.grpInfo.Controls.Add(Me.Label2)
        Me.grpInfo.Controls.Add(Me.Label3)
        Me.grpInfo.Controls.Add(Me.Label4)
        Me.grpInfo.Controls.Add(Me.Label5)
        resources.ApplyResources(Me.grpInfo, "grpInfo")
        Me.grpInfo.Name = "grpInfo"
        Me.grpInfo.TabStop = False
        '
        'Label8
        '
        resources.ApplyResources(Me.Label8, "Label8")
        Me.Label8.Name = "Label8"
        '
        'Label7
        '
        resources.ApplyResources(Me.Label7, "Label7")
        Me.Label7.Name = "Label7"
        '
        'pbIcon
        '
        Me.pbIcon.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        resources.ApplyResources(Me.pbIcon, "pbIcon")
        Me.pbIcon.Name = "pbIcon"
        Me.pbIcon.TabStop = False
        '
        'mtbSize
        '
        resources.ApplyResources(Me.mtbSize, "mtbSize")
        Me.mtbSize.Name = "mtbSize"
        Me.mtbSize.ReadOnly = True
        '
        'txtDesc
        '
        resources.ApplyResources(Me.txtDesc, "txtDesc")
        Me.txtDesc.Name = "txtDesc"
        '
        'txtAuthor
        '
        resources.ApplyResources(Me.txtAuthor, "txtAuthor")
        Me.txtAuthor.Name = "txtAuthor"
        '
        'txtYear
        '
        resources.ApplyResources(Me.txtYear, "txtYear")
        Me.txtYear.Name = "txtYear"
        '
        'txtName
        '
        resources.ApplyResources(Me.txtName, "txtName")
        Me.txtName.Name = "txtName"
        '
        'label6
        '
        resources.ApplyResources(Me.label6, "label6")
        Me.label6.Name = "label6"
        '
        'mtbChksum
        '
        resources.ApplyResources(Me.mtbChksum, "mtbChksum")
        Me.mtbChksum.Name = "mtbChksum"
        Me.mtbChksum.ReadOnly = True
        '
        'grpInput
        '
        Me.grpInput.Controls.Add(Me.rdbMouse)
        Me.grpInput.Controls.Add(Me.rdbJoy)
        resources.ApplyResources(Me.grpInput, "grpInput")
        Me.grpInput.Name = "grpInput"
        Me.grpInput.TabStop = False
        '
        'rdbMouse
        '
        resources.ApplyResources(Me.rdbMouse, "rdbMouse")
        Me.rdbMouse.Name = "rdbMouse"
        Me.rdbMouse.UseVisualStyleBackColor = True
        '
        'rdbJoy
        '
        resources.ApplyResources(Me.rdbJoy, "rdbJoy")
        Me.rdbJoy.Name = "rdbJoy"
        Me.rdbJoy.UseVisualStyleBackColor = True
        '
        'Form1
        '
        resources.ApplyResources(Me, "$this")
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.grpInput)
        Me.Controls.Add(Me.grpInfo)
        Me.Controls.Add(Me.grpDevice)
        Me.Controls.Add(Me.grpChip)
        Me.Controls.Add(Me.MenuStrip1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.MainMenuStrip = Me.MenuStrip1
        Me.Name = "Form1"
        Me.MenuStrip1.ResumeLayout(False)
        Me.MenuStrip1.PerformLayout()
        Me.grpChip.ResumeLayout(False)
        Me.grpChip.PerformLayout()
        Me.grpDevice.ResumeLayout(False)
        Me.grpDevice.PerformLayout()
        Me.grpInfo.ResumeLayout(False)
        Me.grpInfo.PerformLayout()
        CType(Me.pbIcon, System.ComponentModel.ISupportInitialize).EndInit()
        Me.grpInput.ResumeLayout(False)
        Me.grpInput.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents OpenFileDialog1 As System.Windows.Forms.OpenFileDialog
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents MenuStrip1 As System.Windows.Forms.MenuStrip
    Friend WithEvents FileToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents OpenToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents SaveToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents SaveAsToolStripMenuItem1 As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents UzeToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents HexToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents ExitToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents grpChip As System.Windows.Forms.GroupBox
    Friend WithEvents rdb1284 As System.Windows.Forms.RadioButton
    Friend WithEvents rdb644 As System.Windows.Forms.RadioButton
    Friend WithEvents grpDevice As System.Windows.Forms.GroupBox
    Friend WithEvents rdbJam As System.Windows.Forms.RadioButton
    Friend WithEvents rdbCon As System.Windows.Forms.RadioButton
    Friend WithEvents grpInfo As System.Windows.Forms.GroupBox
    Friend WithEvents mtbChksum As System.Windows.Forms.MaskedTextBox
    Friend WithEvents label6 As System.Windows.Forms.Label
    Friend WithEvents mtbSize As System.Windows.Forms.MaskedTextBox
    Friend WithEvents txtDesc As System.Windows.Forms.TextBox
    Friend WithEvents txtAuthor As System.Windows.Forms.TextBox
    Friend WithEvents txtYear As System.Windows.Forms.TextBox
    Friend WithEvents txtName As System.Windows.Forms.TextBox
    Friend WithEvents SaveFileDialog1 As System.Windows.Forms.SaveFileDialog
    Friend WithEvents pbIcon As System.Windows.Forms.PictureBox
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents HelpToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents AboutToolStripMenuItem As System.Windows.Forms.ToolStripMenuItem
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents grpInput As System.Windows.Forms.GroupBox
    Friend WithEvents rdbMouse As System.Windows.Forms.RadioButton
    Friend WithEvents rdbJoy As System.Windows.Forms.RadioButton

End Class
