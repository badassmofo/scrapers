Option Strict On
Option Explicit On

Imports System.Runtime.InteropServices

Public Class Form1
  Private isRunning As Boolean = False

  Private isPerfCounterSupported As Boolean = False
  Private timerFrequency As Int64 = 0
  Private lastTime As Int64 = 0

  Declare Function QueryPerformanceCounter Lib "Kernel32" (ByRef X As Long) As Short
  Declare Function QueryPerformanceFrequency Lib "Kernel32" (ByRef X As Long) As Short

  Private Const WH_KEYBOARD_LL As Integer = 13
  Private Const WH_MOUSE_LL As Integer = 14

  Private Const WM_KEYUP As Integer = &H101
  Private Const WM_SYSKEYDOWN As Integer = &H104
  Private Const WM_SYSKEYUP As Integer = &H105
  Private Const WM_MOUSEMOVE As Integer = &H200
  Private Const WM_LBUTTONDOWN As Integer = &H201
  Private Const WM_LBUTTONUP As Integer = &H202
  Private Const WM_RBUTTONDOWN As Integer = &H204
  Private Const WM_RBUTTONUP As Integer = &H205
  Private Const WM_MBUTTONDOWN As Integer = &H207
  Private Const WM_MBUTTONUP As Integer = &H208
  Private Const WM_MOUSEWHEEL As Integer = &H20A

  Private keyProc As LowLevelProcDelegate = AddressOf KBHookCallback
  Private mouseProc As LowLevelProcDelegate = AddressOf MSHookCallback
  Private KBhookID As IntPtr
  Private MShookID As IntPtr

  Private Delegate Function LowLevelProcDelegate(ByVal nCode As Integer, ByVal wParam As IntPtr, ByVal lParam As IntPtr) As IntPtr

  <DllImport("user32")>
  Private Shared Function SetWindowsHookEx(ByVal idHook As Integer, ByVal lpfn As LowLevelProcDelegate, ByVal hMod As IntPtr, ByVal dwThreadId As UInteger) As IntPtr
  End Function

  <DllImport("user32.dll")>
  Private Shared Function UnhookWindowsHookEx(ByVal hhk As IntPtr) As <MarshalAs(UnmanagedType.Bool)> Boolean
  End Function

  <DllImport("user32.dll")>
  Private Shared Function CallNextHookEx(ByVal hhk As IntPtr, ByVal nCode As Integer, ByVal wParam As IntPtr, ByVal lParam As IntPtr) As IntPtr
  End Function

  <DllImport("kernel32.dll", CharSet:=CharSet.Unicode)>
  Private Shared Function GetModuleHandle(ByVal lpModuleName As String) As IntPtr
  End Function

  Dim kc As New KeysConverter

  Function getTime() As Int64
    If isPerfCounterSupported Then
      Dim tickCount As Int64 = 0
      QueryPerformanceCounter(tickCount)
      Return tickCount
    Else
      Return CType(Environment.TickCount, Int64)
    End If
  End Function

  Sub New()
    InitializeComponent()
    KBhookID = SetHook(keyProc, WH_KEYBOARD_LL)
    MShookID = SetHook(mouseProc, WH_MOUSE_LL)

    If QueryPerformanceFrequency(timerFrequency) <> 0 AndAlso timerFrequency <> 1000 Then
      isPerfCounterSupported = True
    Else
      timerFrequency = 1000
    End If

    lastTime = getTime()
  End Sub

  Sub UpdateTime()
    Dim newTime As Int64 = getTime()
    ListBox1.Items.Add(String.Format("SL:" + ((newTime - lastTime) / timerFrequency).ToString()))
    lastTime = newTime
  End Sub

  Private Sub Form1_FormClosing(ByVal sender As Object, ByVal e As FormClosingEventArgs) Handles Me.FormClosing
    UnhookWindowsHookEx(KBhookID)
    UnhookWindowsHookEx(MShookID)
  End Sub

  Private Function SetHook(ByVal proc As LowLevelProcDelegate, ByVal id As Integer) As IntPtr
    Using curProcess As Process = Process.GetCurrentProcess()
      Using curModule As ProcessModule = curProcess.MainModule
        Return SetWindowsHookEx(id, proc, GetModuleHandle(curModule.ModuleName), 0)
      End Using
    End Using
  End Function

  Private Function KBHookCallback(ByVal nCode As Integer, ByVal wParam As IntPtr, ByVal lParam As IntPtr) As IntPtr
    If nCode >= 0 Then
      Dim vkCode As Integer = Marshal.ReadInt32(lParam)
      UpdateTime()
      ListBox1.Items.Add(kc.ConvertToString(vkCode))
      ListBox1.SelectedIndex = ListBox1.Items.Count - 1
    End If
    Return CallNextHookEx(KBhookID, nCode, wParam, lParam)
  End Function

  Private Function MSHookCallback(ByVal nCode As Integer, ByVal wParam As IntPtr, ByVal lParam As IntPtr) As IntPtr
    If nCode >= 0 Then
      If isRunning Then
        Select Case wParam.ToInt32
          Case WM_MOUSEMOVE
            UpdateTime()
            ListBox1.Items.Add(String.Format("MM:{0},{1}", Cursor.Position.X, Cursor.Position.Y))
          Case WM_LBUTTONDOWN
            UpdateTime()
            ListBox1.Items.Add(String.Format("MD1:{0},{1}", Cursor.Position.X, Cursor.Position.Y))
          Case WM_LBUTTONUP
            UpdateTime()
            ListBox1.Items.Add(String.Format("MU1:{0},{1}", Cursor.Position.X, Cursor.Position.Y))
          Case WM_RBUTTONDOWN
            UpdateTime()
            ListBox1.Items.Add(String.Format("MD2:{0},{1}", Cursor.Position.X, Cursor.Position.Y))
          Case WM_RBUTTONUP
            UpdateTime()
            ListBox1.Items.Add(String.Format("MU2:{0},{1}", Cursor.Position.X, Cursor.Position.Y))
          Case WM_MBUTTONDOWN
            UpdateTime()
            ListBox1.Items.Add(String.Format("MD3:{0},{1}", Cursor.Position.X, Cursor.Position.Y))
          Case WM_MBUTTONUP
            UpdateTime()
            ListBox1.Items.Add(String.Format("MU3:{0},{1}", Cursor.Position.X, Cursor.Position.Y))
          Case WM_MOUSEWHEEL
        End Select
        ListBox1.SelectedIndex = ListBox1.Items.Count - 1
      End If
    End If
    Return CallNextHookEx(MShookID, nCode, wParam, lParam)
  End Function

  Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
    If isRunning Then
      isRunning = False
      Button1.Text = "REC"
    Else
      isRunning = True
      Button1.Text = "STOP"
    End If
    Button2.Enabled = Not isRunning
    Button3.Enabled = Not isRunning
  End Sub

  Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
    ListBox1.Items.Clear()
  End Sub

  Private Sub Button3_Click(sender As Object, e As EventArgs) Handles Button3.Click
    If ListBox1.Items.Count = 0 Then
      MessageBox.Show("Nothing to do...")
      Return
    End If
  End Sub

  Private Sub ListBox1_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles ListBox1.KeyDown
    If e.KeyCode = Keys.Back And ListBox1.SelectedIndex > 0 And Not isRunning Then
      ListBox1.Items.RemoveAt(ListBox1.SelectedIndex)
    End If
  End Sub
End Class
