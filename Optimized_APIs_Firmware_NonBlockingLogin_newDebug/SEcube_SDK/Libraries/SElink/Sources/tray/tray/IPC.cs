using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace tray {

    /// <summary>
    /// Implements the service's IPC protocol, used to configure devices and update filters.
    /// A named pipe is used for communicating.
    /// </summary>
    public class IPC {

        private const string PipeName =
            "b6fb44de3444904803bd4ee1039a2af942ebbede24ddc680f3bb80090b6b7dc7";

        // ---- Base protocol constants and structures ----

        public const int 
            RequestMax = 64,
            ResponseMax = 16*1024,
            SerialNumberSize = 32,
            PinMax = 32;

        /// <summary>
        /// Command field constant definitions for Request
        /// </summary>
        private struct Command {
            public const uint
                Reload = 100,
                Status = 200,
                Discover = 500,
                SetDevice = 501,
                Reset = 502;
        }

        /// <summary>
        /// Request structure for the SELink service IPC protocol
        /// </summary>
        private class Request {
            public Request() { }
            public Request(uint command) {
                this.command = command;
            }
            public uint command = 0;
            public byte[] data = new byte[0];
            public byte[] GetBytes()
            {
                if (data == null || data.Length > RequestMax) return null;
                byte[] ret = new byte[4 + 4 + RequestMax];
                BitConverter.GetBytes(command).CopyTo(ret, 0);
                BitConverter.GetBytes((uint)data.Length).CopyTo(ret, 4);
                data.CopyTo(ret, 8);
                return ret;
            }
        }

        /// <summary>
        /// Status field constant definitions for Response
        /// </summary>
        public struct ResponseStatus {
            public const uint
                Error = 0xFFFFFFFF,
                Ok = 0,
                NotAvailable = 1;
        }

        /// <summary>
        /// Response structure for the SELink service IPC protocol
        /// </summary>
        public class Response {
            public Response() { }
            public Response(byte[] hdr)
            {
                status = BitConverter.ToUInt32(hdr, 0);
                uint size = BitConverter.ToUInt32(hdr, 4);
                data = (size > ResponseMax) ? (null) : (new byte[size]);
            }
            public bool IsError()
            {
                return (status == ResponseStatus.Error);
            }
            public uint status = ResponseStatus.Error;
            public byte[] data = new byte[0];
        }

        /// <summary>
        /// Send a request to the service and read its response
        /// </summary>
        /// <param name="req">Request to be sent</param>
        /// <returns>Response object</returns>
        private Response Query(Request req)
        {
            try {
                using (NamedPipeClientStream pc =
                    new NamedPipeClientStream(".", PipeName, PipeDirection.InOut)) {
                    pc.Connect(0);

                    byte[] buf = req.GetBytes();
                    if (buf == null) return new Response();
                    BitConverter.GetBytes((uint)req.command).CopyTo(buf, 0);
                    BitConverter.GetBytes((uint)req.data.Length).CopyTo(buf, 4);
                    if (req.data.Length > 0) req.data.CopyTo(buf, 8);
                    pc.Write(buf, 0, buf.Length);
                    pc.Flush();

                    pc.Read(buf, 0, 8);
                    Response resp = new Response(buf);
                    if (resp.data == null) return new Response();

                    pc.Read(resp.data, 0, resp.data.Length);
                    return resp;
                }
            }
            catch (Exception e) {
                return new Response();
            }
        }

        // ---- Discover ----

        /// <summary>
        /// Discover command response
        /// </summary>
        public class Device {
            public Device()
            { }

            public Device(string root, byte[] serialNumber)
            {
                this.root = root;
                this.serialNumber = serialNumber;
            }

            /// <summary>
            /// Get a hex string representation of the serial number
            /// </summary>
            /// <returns>Hex string</returns>
            public string SerialNumberAsHex()
            {
                StringBuilder hex = new StringBuilder(serialNumber.Length * 2);
                foreach (byte b in serialNumber)
                    hex.AppendFormat("{0:x2}", b);
                return hex.ToString();
            }

            public byte[] serialNumber = new byte[SerialNumberSize];
            public string root;
        }

        /// <summary>
        /// Issue Discover command
        /// </summary>
        /// <returns>List of discovered devices</returns>
        public List<Device> Discover()
        {
            List<Device> devices = new List<Device>();
            Response resp = Query(new Request(Command.Discover));
            try {
                if (!resp.IsError()) {
                    // extract list of devices from data blob
                    using (MemoryStream mem = new MemoryStream(resp.data)) {
                        using (BinaryReader sr = new BinaryReader(mem)) {
                            mem.Seek(0, SeekOrigin.Begin);
                            int len = (int)resp.data.Length;
                            while (len > 0) {
                                // extract serial number
                                byte[] dsn = new byte[SerialNumberSize];
                                sr.Read(dsn, 0, SerialNumberSize);
                                len -= 32;

                                // extract path length
                                byte path_len = sr.ReadByte();
                                len -= 1;

                                // extract path string
                                byte[] path_utf8 = new byte[path_len];
                                sr.Read(path_utf8, 0, path_len);
                                len -= path_len;
                                string path = System.Text.Encoding.UTF8.GetString(path_utf8);

                                devices.Add(new Device(path, dsn));
                            }
                        }
                    }
                }
            }
            catch (Exception) { }
            return devices;
        }


        // ---- Status ----

        /// <summary>
        /// Provider type field constants for Status command
        /// </summary>
        public struct ProviderType {
            public const uint
                Soft = 0,
                SECube = 1,
                SE3 = 2;
        }

        /// <summary>
        /// Status command response
        /// </summary>
        public class ProviderStatus {

            public ProviderStatus()
            { }

            /// <summary>
            /// Interpret response as Status command response
            /// </summary>
            /// <param name="resp">Response object</param>
            public ProviderStatus(Response resp)
            {
                if (resp.data.Length != 8 || resp.status != ResponseStatus.Ok) return;
                status = resp.status;
                providerType = BitConverter.ToUInt32(resp.data, 0);
                providerStatus = BitConverter.ToUInt32(resp.data, 4);
            }

            public uint status = ResponseStatus.Error;
            public uint providerType = 0;
            public uint providerStatus = 0;
            
            public const uint
                LoggedIn = 0,
                WaitConfig = 1,
                ErrorNotFound = 100,
                ErrorUserPin = 200,
                ErrorDevice = 201,
                ErrorUnknown = 302;

            /// <summary>
            /// Map each status code to a message
            /// </summary>
            public static readonly Dictionary<uint, string> providerStatusMessages = 
                new Dictionary<uint, string>() {
                    { LoggedIn, Resources.strings.provider_logged_in },
                    { WaitConfig, Resources.strings.provider_wait_config },
                    { ErrorNotFound, Resources.strings.provider_error_notfound },
                    { ErrorUserPin, Resources.strings.provider_error_userpin },
                    { ErrorDevice, Resources.strings.provider_error_device },
                    { ErrorUnknown, Resources.strings.provider_error_unknown }
                };
        }

        /// <summary>
        /// Issue Status Command, to get information about the device status
        /// </summary>
        /// <returns>true on success</returns>
        public ProviderStatus Status()
        {
            List<Device> devices = new List<Device>();
            Response resp = Query(new Request(Command.Status));
            return new ProviderStatus(resp);
        }


        // ---- Reload ----

        /// <summary>
        /// Issue Reload command
        /// </summary>
        /// <returns>true on success</returns>
        public bool Reload()
        {
            Response resp = Query(new Request(Command.Reload));
            return (resp.status == ResponseStatus.Ok);
        }


        // ---- Reset ----

        /// <summary>
        /// Issue Reset Command, to disconnect from the currently connected device
        /// </summary>
        /// <returns>true on success</returns>
        public uint Reset()
        {
            Response resp = Query(new Request(Command.Reset));
            return resp.status;
        }
        

        // ---- SetDevice ----

        /// <summary>
        /// Issue SetDevice command, to connect to a device
        /// </summary>
        /// <param name="dsn">Serial Number of the device, 32 bytes</param>
        /// <param name="pin">User Pin, up to 32 bytes</param>
        /// <returns></returns>
        public bool SetDevice(byte[] dsn, byte[] pin)
        {
            Request req = new Request(Command.SetDevice);
            if(dsn.Length != SerialNumberSize || pin.Length > PinMax) {
                return false;
            }
            req.data = new byte[SerialNumberSize + pin.Length];
            dsn.CopyTo(req.data, 0);
            pin.CopyTo(req.data, SerialNumberSize);
            Response resp = Query(req);

            return (!resp.IsError());
        }

    }
}
