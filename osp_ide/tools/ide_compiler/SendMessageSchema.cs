using System.IO;
using System.Reflection;

namespace ide_compiler
{
    static class SendMessageSchema
    {
        public static StreamReader ToStreamReader(this Stream stream)
        {
            return new StreamReader(stream);
        }

        public static string Get()
        {
            return Assembly
                .GetExecutingAssembly()
                .GetManifestResourceStream("ide_compiler.SendMessageSchema.json")
                .ToStreamReader()
                .ReadToEnd();
        }
    }
}
