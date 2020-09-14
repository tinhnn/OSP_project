using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json.Schema;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace osp.dbc
{
    static class Compiler
    {
        public static void Compile(TextReader input, TextWriter output)
        {
            TextWriter outputw;
            var schemaDocument = CommandSchema.Get();
            dynamic schemaJson = JsonConvert.DeserializeObject(schemaDocument);
            var commandSchemaDocument = JsonConvert.SerializeObject(
                schemaJson.command);
            var schema = JSchema.Parse((string)commandSchemaDocument);
            var jsons = JToken.Parse(input.ReadToEnd());
            IList<string> errorMessages;
            var isValid = jsons.IsValid(schema, out errorMessages);
            if (isValid == false)
            {
                throw new Exception(string.Join("\n", errorMessages.ToArray()));
            }
            foreach(var json in jsons)
            {
                CommandSourceBase src = new CommandSourceBase();
                var source = new CommandSource(json);
                dynamic _json = json;
                string filename = src.ToStringHelper.ToStringWithCulture(_json.name) + ".aml";
                outputw = File.CreateText(filename);
                outputw.Write(source.TransformText());
                outputw.Flush();
                Console.WriteLine(" -> .\\{0}", filename);
            }
        }
    }
}
