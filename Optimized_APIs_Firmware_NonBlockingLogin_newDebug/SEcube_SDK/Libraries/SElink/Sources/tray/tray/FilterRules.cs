using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Net;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;
using System.Threading;

namespace tray {

    /// <summary>
    /// Filter rules operations:
    /// - load from json
    /// - save to json
    /// - validate fields
    /// </summary>
    class FilterRules {

        /// <summary>
        /// JSON binding: element of "rules" array
        /// </summary>
        [DataContract]
        public class FilterRule {
            [DataMember(Name = "description", Order = 0)]
            public string Description;
            [DataMember(EmitDefaultValue = false, Name = "path", Order = 1)]
            public string ExePath;
            [DataMember(EmitDefaultValue = false, Name = "address", Order = 2)]
            public string Address;
            [DataMember(EmitDefaultValue = false, Name = "port-first", Order = 3)]
            public int? PortFirst;
            [DataMember(EmitDefaultValue = false, Name = "port-last", Order = 4)]
            public int? PortLast;
            [DataMember(Name = "action", Order = 5)]
            public string Action;
            [DataMember(EmitDefaultValue = false, Name = "key-id", Order = 6)]
            public int? KeyId;
        };

        /// <summary>
        /// JSON binding: root node
        /// </summary>
        [DataContract]
        public class FilterConfig {
            [DataMember(Name = "rules")]
            public List<FilterRule> Rules = new List<FilterRule>();
        }

        /// <summary>
        /// Stored filter rules
        /// </summary>
        public FilterConfig conf = new FilterConfig();

        /// <summary>
        /// Validate the ip address field of a filter rule
        /// </summary>
        /// <param name="Address">Address field as string</param>
        /// <returns>true if the address is valid</returns>
        public static bool IsValidAddress(string Address)
        {
            if (Address.Equals("")) return true;
            IPAddress tmp;
            return IPAddress.TryParse(Address, out tmp);
        }

        /// <summary>
        /// Validate the port range in a filter rule
        /// </summary>
        /// <param name="PortFirst">First port field as string</param>
        /// <param name="PortLast">Last port field as string</param>
        /// <returns>true if the port range is valid</returns>
        public static bool IsValidRange(string PortFirst, string PortLast)
        {
            int iPortFirst = -1, iPortLast = -1;
            if (PortFirst.Equals("") && PortLast.Equals("")) return true;
            if (!int.TryParse(PortFirst, out iPortFirst) || !int.TryParse(PortLast, out iPortLast)) return false;
            if (iPortFirst > iPortLast) return false;
            if (iPortFirst < 1 || iPortLast >= 256 * 256) return false;
            return true;
        }

        /// <summary>
        /// Validate the description field of a filter rule, checking whether its length
        /// exceeds the maximum limit
        /// </summary>
        /// <param name="Description">Description field as string</param>
        /// <returns>true if the description is valid</returns>
        public static bool IsValidDescription(string Description)
        {
            return (Description.Length < 64);
        }

        /// <summary>
        /// Validate the action associated to a filter rule
        /// </summary>
        /// <param name="Action">Action field as string</param>
        /// <param name="KeyId">Key Id field as string</param>
        /// <returns></returns>
        public static bool IsValidAction(string Action, string KeyId)
        {
            if (Action.Equals("block") || Action.Equals("allow")) {
                return KeyId.Equals("");
            }
            else if (Action.Equals("encrypt")) {
                int iKeyId = -1;
                if (!int.TryParse(KeyId, out iKeyId)) return false;
                if (iKeyId < 0) return false;
                return true;
            }
            return false;
        }

        /// <summary>
        /// Replace empty string with null string
        /// </summary>
        /// <param name="x">string</param>
        /// <returns>null if string is empty, else string</returns>
        static private string IfNotEmpty(string x)
        {
            return (x.Equals("") ? null : x);
        }
        /// <summary>
        /// Parse a string to integer
        /// </summary>
        /// <param name="x">string</param>
        /// <returns>parsed integer, or null if parsing fails</returns>
        static private int? IfInteger(string x)
        {
            int y = 0;
            if (int.TryParse(x, out y)) return y;
            else return null;
        }

        /// <summary>
        /// Add a filter rule to the list.
        /// The rule will only be added if all the fields are valid.
        /// </summary>
        /// <param name="Description">Description field</param>
        /// <param name="ExePath">Excecutable Path field</param>
        /// <param name="Address">Address field</param>
        /// <param name="PortFirst">First Port field</param>
        /// <param name="PortLast">Last Port field</param>
        /// <param name="Action">Action field</param>
        /// <param name="KeyId">Key Id field</param>
        /// <returns>true if all the fields are valid</returns>
        public bool Add(string Description, string ExePath, string Address,
            string PortFirst, string PortLast, string Action, string KeyId)
        {
            bool success = true;
            FilterRule fl = new FilterRule();

            // validate fields
            success = success && IsValidRange(PortFirst, PortLast);
            success = success && IsValidAction(Action, KeyId);
            success = success && IsValidDescription(Description);
            success = success && IsValidAddress(Address);
            if (!success) return false;

            // parse fields and fill rule structure
            fl.Action = Action;
            fl.Address = IfNotEmpty(Address);
            fl.Description = Description;
            fl.ExePath = IfNotEmpty(ExePath.Replace(@"\", "/"));
            fl.KeyId = IfInteger(KeyId);
            fl.PortFirst = IfInteger(PortFirst);
            fl.PortLast = IfInteger(PortLast);

            conf.Rules.Add(fl);
            return true;
        }

        /// <summary>
        /// Get the Location of the filter configuration file
        /// </summary>
        /// <returns></returns>
        string GetFilterConfigPath()
        {
            return System.IO.Path.Combine(
                System.IO.Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location),
                "selinksvc.json");
        }

        readonly DataContractJsonSerializerSettings Settings =
            new DataContractJsonSerializerSettings { UseSimpleDictionaryFormat = true };

        /// <summary>
        /// Read filter rules from the configuration file
        /// </summary>
        /// <returns>true on success</returns>
        public bool Read()
        {
            try {
                string path = GetFilterConfigPath();
                using (var stream = File.OpenRead(path)) {
                    var currentCulture = Thread.CurrentThread.CurrentCulture;
                    Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;

                    try {
                        var serializer = new DataContractJsonSerializer(typeof(FilterConfig), Settings);
                        var item = (FilterConfig)serializer.ReadObject(stream);
                        if (Equals(item, null)) throw new Exception();
                        conf = item;
                        foreach (FilterRule rule in conf.Rules) {
                            if (rule.ExePath != null) {
                                rule.ExePath = rule.ExePath.Replace("/", @"\");
                            }
                        }
                        return true;
                    }
                    catch (Exception exception) {
                        Debug.WriteLine(exception.ToString());
                        return false;
                    }
                    finally {
                        Thread.CurrentThread.CurrentCulture = currentCulture;
                    }
                }
            }
            catch {
                return false;
            }
        }

        /// <summary>
        /// Save filter rules to the configuration file
        /// </summary>
        /// <returns>true on success</returns>
        public bool Save()
        {
            try {
                string path = GetFilterConfigPath();
                using (var stream = File.Open(path, FileMode.Create)) {
                    var currentCulture = Thread.CurrentThread.CurrentCulture;
                    Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;
                    try {
                        using (var writer = JsonReaderWriterFactory.CreateJsonWriter(
                            stream, Encoding.UTF8, true, true, "  ")) {
                            var serializer = new DataContractJsonSerializer(typeof(FilterConfig), Settings);
                            serializer.WriteObject(writer, conf);
                            writer.Flush();
                        }
                    }
                    catch (Exception exception) {
                        Debug.WriteLine(exception.ToString());
                        return false;
                    }
                    finally {
                        Thread.CurrentThread.CurrentCulture = currentCulture;
                    }
                }
            }
            catch (Exception exception) {
                Debug.WriteLine(exception.ToString());
                return false;
            }
            return true;
        }


        public class Actions {
            /// <summary>
            /// Possible action codes in filter rules
            /// </summary>
            public static readonly string[] Codes = new string[] { "allow", "block", "encrypt" };
            /// <summary>
            /// Localized names of the action codes in filter rules 
            /// </summary>
            public static readonly string[] Names;
            /// <summary>
            /// Default action code
            /// </summary>
            public static readonly string Default;

            private static readonly Dictionary<string, string> actionMap, actionMapReverse;
            
            // get localized strings from resources
            private static string[] localizedActionStrings = tray.Resources.strings.wr_actions.Split(',');

            static Actions()
            {
                // initialize (code, name) and (name, code) maps
                actionMap = new Dictionary<string, string>();
                actionMapReverse = new Dictionary<string, string>();
                if (localizedActionStrings.Length != Codes.Length) {
                    localizedActionStrings = Codes;
                }
                Names = localizedActionStrings;
                for (int i = 0; i < Codes.Length; i++) {
                    actionMap.Add(Codes[i], Names[i]);
                }
                for (int i = 0; i < Codes.Length; i++) {
                    actionMapReverse.Add(Names[i], Codes[i]);
                }
                // default is allow(0)
                Default = Codes[0];
            }

            /// <summary>
            /// Get localized name from action code
            /// </summary>
            /// <param name="name">name</param>
            /// <returns>code, or empty string if the name is not valid</returns>
            public static string GetCode(string name)
            {
                string code = "";
                actionMapReverse.TryGetValue(name, out code);
                if (code == null) code = "";
                return code;
            }

            /// <summary>
            /// Get action code from localized name
            /// </summary>
            /// <param name="code">code</param>
            /// <returns>name, or empty string if the code is not valid</returns>
            public static string GetName(string code)
            {
                string name = "";
                actionMap.TryGetValue(code, out name);
                if (name == null) name = "";
                return name;
            }

        }
        

    }
}
