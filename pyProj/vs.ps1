Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing

$form = New-Object System.Windows.Forms.Form
$form.Text = "Visual PowerShell"
$form.Size = New-Object System.Drawing.Size(300,200)
$form.StartPosition = "CenterScreen"

$label = New-Object System.Windows.Forms.Label
$label.Text = "Hello from PowerShell GUI!"
$label.AutoSize = $true
$label.Location = New-Object System.Drawing.Point(50,50)
$form.Controls.Add($label)

$form.Add_Shown({$form.Activate()})
$form.ShowDialog()