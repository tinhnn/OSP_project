using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json.Schema;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace ide_compiler
{
    static class Compiler
    {
        public enum TypeCompile
        {
            OneShoot = 0,
            Periodic
        }
        public static void Compile(TypeCompile typeCompile, TextReader input, TextWriter output, dynamic mapOdb)
        {
            var schemaDocument = SendMessageSchema.Get();
            dynamic schemaJson = JsonConvert.DeserializeObject(schemaDocument);
            var commandSchemaDocument = JsonConvert.SerializeObject(schemaJson.list_message);
            var schema = JSchema.Parse((string)commandSchemaDocument);
            var json = JToken.Parse(input.ReadToEnd());
            IList<string> errorMessages;
            var isValid = json.IsValid(schema, out errorMessages);
            if (isValid == false)
            {
                throw new Exception(string.Join("\n", errorMessages.ToArray()));
            }

            int nCount = json.Count();

            var jsonMapOdb = JToken.Parse(mapOdb.ReadToEnd());
            switch (typeCompile)
            {
                case TypeCompile.OneShoot:
                    var sourceOnsShoot = new SendOneShoot(json, jsonMapOdb);
                    output.Write(sourceOnsShoot.TransformText());
                    break;
                case TypeCompile.Periodic:
                    var sourcePeriodic = new SendPeriodic(json, jsonMapOdb);
                    output.Write(sourcePeriodic.TransformText());
                    break;
                default:
                    break;
            }
            output.Flush();
            output.Close();
        }
    }
}