using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json.Schema;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WpfApplication3
{
    public class ReadAndCheckData
    {
        //  Read and check json command with schema file
        public bool CheckCommand(string input)
        {
            var schema = JSchema.Parse(CommandSchema.Get());
            var json = JToken.Parse(input);

            IList<string> errorMessages;
            var isValid = json.IsValid(schema, out errorMessages);
            return isValid;
            //if (isValid == false)
            //{
            //    throw new Exception(string.Join("\n", errorMessages.ToArray()));
            //}
        }

        //  Read and check json send message with schema file
        public bool CheckSendMessage(string input)
        {
            
            if (String.Equals("[]", input, StringComparison.CurrentCulture))
            {
                // not check
                return true;
            }

            var schema = JSchema.Parse(SendMessageSchema.Get());
            var json = JToken.Parse(input);

            IList<string> errorMessages;
            var isValid = json.IsValid(schema, out errorMessages);
            if (isValid == false)
            {
                throw new Exception(string.Join("\n", errorMessages.ToArray()));
            }
            return isValid;
        }

    }

}
