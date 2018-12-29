Option Strict On
Option Explicit On

Imports System.Runtime.InteropServices
Imports System.Threading
Imports System.Drawing.Graphics

Public Structure RECT
  Public left, top, right, bottom As Integer
End Structure

Public Class Form1
  <DllImport("user32.dll")>
  Private Shared Function SetWindowLong(ByVal hwnd As IntPtr, ByVal index As Integer, ByVal new_long As Integer) As Integer
  End Function

  <DllImport("user32.dll", SetLastError:=True)>
  Private Shared Function GetWindowLong(ByVal hwnd As IntPtr, ByVal index As Integer) As Integer
  End Function

  <DllImport("user32.dll", SetLastError:=True)>
  Private Shared Function GetWindowRect(ByVal hwnd As IntPtr, <Out> ByRef rect As RECT) As Boolean
  End Function

  Public win_rect As RECT
  Public win_proc As Process
  Public win_hndl As IntPtr
  Public context As Graphics
  Public times As Font = New Font("Times New Roman", 18)
  Public brush As Brush = New SolidBrush(Color.White)

  Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
    Timer1.Start()

    Dim style As Integer = GetWindowLong(Me.Handle, -20)
    SetWindowLong(Me.Handle, -20, (style Or &H80000 Or &H20))

    Me.TopMost = True
    Me.FormBorderStyle = FormBorderStyle.None
    Me.BackColor = System.Drawing.Color.Black
    Me.TransparencyKey = System.Drawing.Color.Black

    Me.DoubleBuffered = True
    SetStyle(ControlStyles.OptimizedDoubleBuffer, True)
  End Sub

  Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick
    For Each p As Process In Process.GetProcesses()
      win_hndl = p.MainWindowHandle
      If win_hndl <> IntPtr.Zero And p.ProcessName.CompareTo("TekkenGame-Win64-Shipping") = 0 Then
        win_proc = p
        Timer1.Stop()
        Timer2.Start()
        Debug.WriteLine("Tekken 7 (" & win_proc.ProcessName & ") found! PID: " & win_proc.Id)
        Return
      End If
    Next
    Debug.WriteLine("Tekken 7 isn't running...")
  End Sub

  Private Sub Timer2_Tick(sender As Object, e As EventArgs) Handles Timer2.Tick
    If win_proc.HasExited Then
      Timer2.Stop()
      Timer1.Start()
      Return
    End If

    win_proc.Refresh()
    win_hndl = win_proc.MainWindowHandle
    GetWindowRect(win_hndl, win_rect)
    Me.Top = win_rect.top
    Me.Left = win_rect.left
    Me.Size = New Size(win_rect.right - win_rect.left, win_rect.bottom - win_rect.top)

    Me.Refresh()
  End Sub

  Private Sub Form1_Paint(sender As Object, e As PaintEventArgs) Handles MyBase.Paint
    If Not Timer2.Enabled Then
      Return
    End If

    context = e.Graphics

    context.DrawString(Date.Now.ToString, times, brush, 10, 10)
  End Sub
End Class
