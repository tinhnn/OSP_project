using System.IO;
using System.Reflection;

namespace WpfApplication3
{
    static class CommandSchema
    {
        public static string Get()
        {
            using (StreamReader r = new StreamReader("CommandSchema.json"))
            {
                string json = r.ReadToEnd();
                return json;
            }
        }
    }

    static class SendMessageSchema
    {
        public static string Get()
        {
            using (StreamReader r = new StreamReader("SendMessageSchema.json"))
            {
                string json = r.ReadToEnd();
                return json;
            }
        }
    }
}
