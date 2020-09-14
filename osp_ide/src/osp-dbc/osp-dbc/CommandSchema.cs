using System.IO;
using System.Reflection;

namespace osp.dbc
{
    static class CommandSchema
    {
        public static StreamReader ToStreamReader(this Stream stream)
        {
            return new StreamReader(stream);
        }

        public static string Get()
        {
            return Assembly
                .GetExecutingAssembly()
                .GetManifestResourceStream("osp.dbc.CommandSchema.json")
                .ToStreamReader()
                .ReadToEnd();
        }
    }
}
