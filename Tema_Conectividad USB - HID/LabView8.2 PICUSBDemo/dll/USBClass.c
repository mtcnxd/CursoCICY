using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using HIDClass;

namespace USB_PWM_Laser_Controller
{
    unsafe class USBGenericHID
    {
        private uint VID = (uint)0;
        private uint PID = (uint)0;
        private uint BufferSize = (uint)0;
        private uint Timeout = (uint)0;
        private bool TimeoutOccurred = (bool)false;

        Object LockObject = new Object();

        public USBGenericHID(uint VID, uint PID, uint BufferSize, uint Timeout)
        {
            this.VID = VID;
            this.PID = PID;
            this.BufferSize = BufferSize;
            this.Timeout = Timeout;

            MCHPHIDClass.HIDClassInit(VID, PID, BufferSize, Timeout);
        }

        private void TimerTick(object sender, EventArgs e)
        {
            TimeoutOccurred = (bool)true;
        }

        public uint GetVID()
        {
            return (uint)this.VID;
        }

        public uint GetPID()
        {
            return (uint)this.PID;
        }

        public uint GetBufferSize()
        {
            return (uint)this.BufferSize;
        }

        public uint GetTimeout()
        {
            return (uint)this.Timeout;
        }

        public bool USBGenericHIDIsConnected()
        {
            return (bool)MCHPHIDClass.HIDIsConnected();
        }

        public unsafe bool USBGenericHIDSend(byte[] DataOut)
        {
            if (DataOut == (byte[])null)
                return (bool)false;

            if (DataOut.Length > (int)this.BufferSize)
                return (bool)false;

            byte* TempData = stackalloc byte[(int)this.BufferSize];

            for (int i = 0; i < (int)DataOut.Length; i++)
                TempData[i] = (byte)DataOut[i];

            return (bool)MCHPHIDClass.HIDWriteReport((byte*)TempData, (uint)DataOut.Length);
        }

        public unsafe bool USBGenericHIDSend(char[] DataOut)
        {
            if (DataOut == (char[])null)
                return (bool)false;

            if (DataOut.Length > (int)this.BufferSize)
                return (bool)false;

            byte* TempData = stackalloc byte[(int)this.BufferSize];
            
            for (int i = 0; i < (int)DataOut.Length; i++)
                TempData[i] = (byte)DataOut[i];

            return (bool)MCHPHIDClass.HIDWriteReport((byte*)TempData, (uint)DataOut.Length);
        }

        public unsafe int USBGenericHIDSendBulk(LinkedList<byte[]> List, int Timeout)
        {
            int BytesSent = 0;
            Timer T = new Timer();

            if (List == (LinkedList<byte[]>)null)
                return (int)0;

            lock (LockObject)
            {
                if (Timeout > 0)
                {
                    T.Interval = Timeout;
                    T.Tick += new EventHandler(TimerTick);
                    T.Enabled = true;
                }

                foreach(byte[] b in List)
                {
                    if (!USBGenericHIDSend((byte[])b))
                    {
                        T.Enabled = false;
                        TimeoutOccurred = (bool)false;

                        return (int)BytesSent;
                    }
                    else
                    {
                        BytesSent += (int)b.Length;
                    }

                    if (TimeoutOccurred)
                    {
                        T.Enabled = false;
                        TimeoutOccurred = (bool)false;

                        return (int)BytesSent;
                    }
                }

                T.Enabled = false;
                TimeoutOccurred = (bool)false;
            }

            return (int)BytesSent;
        }

        public unsafe int USBGenericHIDSendBulk(LinkedList<char[]> List, int Timeout)
        {
            int BytesSent = 0;
            Timer T = new Timer();

            if (List == (LinkedList<char[]>)null)
                return (int)0;

            lock (LockObject)
            {
                if (Timeout > 0)
                {
                    T.Interval = Timeout;
                    T.Tick += new EventHandler(TimerTick);
                    T.Enabled = true;
                }

                foreach (char[] c in List)
                {
                    if (!USBGenericHIDSend((char[])c))
                    {
                        T.Enabled = false;
                        TimeoutOccurred = (bool)false;

                        return (int)BytesSent;
                    }
                    else
                    {
                        BytesSent += (int)c.Length;
                    }

                    if (TimeoutOccurred)
                    {
                        T.Enabled = false;
                        TimeoutOccurred = (bool)false;

                        return (int)BytesSent;
                    }
                }

                T.Enabled = false;
                TimeoutOccurred = (bool)false;
            }

            return (int)BytesSent;
        }

        public unsafe byte[] USBGenericHIDReceiveByte()
        {
            byte[] TempDataBuffer = new byte[(int)this.BufferSize];

            byte* TempData = stackalloc byte[(int)this.BufferSize];

            if (HIDClass.MCHPHIDClass.HIDReadReport((byte*)TempData))
            {
                for (int i = 0; i < (int)this.BufferSize; i++)
                    TempDataBuffer[i] = (byte)*TempData++;

                return (byte[])TempDataBuffer;
            }
            else
            {
                return (byte[])null;
            }
        }

        public unsafe char[] USBGenericHIDReceiveChar()
        {
            char[] TempDataBuffer = new char[(int)this.BufferSize];

            byte* TempData = stackalloc byte[(int)this.BufferSize];

            if (HIDClass.MCHPHIDClass.HIDReadReport((byte*)TempData))
            {
                for (int i = 0; i < (int)this.BufferSize; i++)
                    TempDataBuffer[i] = (char)*TempData++;

                return (char[])TempDataBuffer;
            }
            else
            {
                return (char[])null;
            }
        }

        public unsafe LinkedList<byte[]> USBGenericHIDReceiveByteBulk(int NumberOfBytes, int NumberOfBufferSizeTransfers, int Timeout)
        {
            // we cannnot have both, just one or the other
            if (NumberOfBytes > (int)0 && NumberOfBufferSizeTransfers > (int)0)
            {
                return (LinkedList<byte[]>)null;
            }

            LinkedList<byte[]> BulkList = new LinkedList<byte[]>();
            Timer T = new Timer();

            int NumberOfBytesToTransfer = (int)0;

            if (NumberOfBufferSizeTransfers > (int)0)
            {
                NumberOfBytesToTransfer = (int)((int)NumberOfBufferSizeTransfers * (int)this.BufferSize);
            }
            else
            {
                NumberOfBytesToTransfer = (int)NumberOfBytes;
            }

            lock (LockObject)
            {
                if (Timeout > 0)
                {
                    T.Interval = Timeout;
                    T.Tick += new EventHandler(TimerTick);
                    T.Enabled = true;
                }

                while (!TimeoutOccurred)
                {
                    BulkList.AddLast((byte[])USBGenericHIDReceiveByte());

                    if (BulkList.Last() == (byte[])null)
                    {
                        // remove it since the timeout occurred
                        BulkList.RemoveLast();

                        // we don't leave the function all together even though a
                        // timeout occurred on that one transfer.  We only leave
                        // once the timeout the user specified in this function occurrs
                    }
                    else
                    {
                        NumberOfBytesToTransfer -= (int)this.BufferSize;

                        if (NumberOfBytesToTransfer == (int)0)
                            break;
                    }
                }

                TimeoutOccurred = (bool)false;
            }

            return (LinkedList<byte[]>)BulkList;
        }

        public unsafe LinkedList<char[]> USBGenericHIDReceiveCharBulk(int NumberOfBytes, int NumberOfBufferSizeTransfers, int Timeout)
        {
            // we cannnot have both, just one or the other
            if (NumberOfBytes > (int)0 && NumberOfBufferSizeTransfers > (int)0)
            {
                return (LinkedList<char[]>)null;
            }

            LinkedList<char[]> BulkList = new LinkedList<char[]>();
            Timer T = new Timer();

            int NumberOfBytesToTransfer = (int)0;

            if (NumberOfBufferSizeTransfers > (int)0)
            {
                NumberOfBytesToTransfer = (int)((int)NumberOfBufferSizeTransfers * (int)this.BufferSize);
            }
            else
            {
                NumberOfBytesToTransfer = (int)NumberOfBytes;
            }

            lock (LockObject)
            {
                if (Timeout > 0)
                {
                    T.Interval = Timeout;
                    T.Tick += new EventHandler(TimerTick);
                    T.Enabled = true;
                }

                while (!TimeoutOccurred)
                {
                    BulkList.AddLast((char[])USBGenericHIDReceiveChar());

                    if (BulkList.Last() == (char[])null)
                    {
                        // remove it since the timeout occurred
                        BulkList.RemoveLast();

                        // we don't leave the function all together even though a
                        // timeout occurred on that one transfer.  We only leave
                        // once the timeout the user specified in this function occurs
                    }
                    else
                    {
                        NumberOfBytesToTransfer -= (int)this.BufferSize;

                        if (NumberOfBytesToTransfer == (int)0)
                            break;
                    }
                }

                TimeoutOccurred = (bool)false;
            }

            return (LinkedList<char[]>)BulkList;
        }
    }
}
