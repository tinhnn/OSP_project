﻿// ------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version: 14.0.0.0
//  
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
// ------------------------------------------------------------------------------
namespace ide_compiler
{
    using System.Linq;
    using System.Text;
    using System.Collections.Generic;
    using Newtonsoft.Json.Linq;
    using System;
    
    /// <summary>
    /// Class to produce the template output
    /// </summary>
    
    #line 1 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
    [global::System.CodeDom.Compiler.GeneratedCodeAttribute("Microsoft.VisualStudio.TextTemplating", "14.0.0.0")]
    public partial class SendPeriodic : SendPeriodicBase
    {
#line hidden
        /// <summary>
        /// Create the template output
        /// </summary>
        public virtual string TransformText()
        {
            this.Write("module periodic_message\r\n\r\ndef get_version()\r\n    ldk \"");
            
            #line 11 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(GetHashString(_json.ToString())));
            
            #line default
            #line hidden
            this.Write(@"""
    ret
end

def create_msg_tuple (msg, port, cycle_time)
    var [msg_tuple]
    imp std-tuple:new/1
    lsi 5
    ivk 1
    slv msg_tuple

    // set counter to tuple
    imp std-tuple:set/3
    lsi 0
    lsi 0
    llv msg_tuple
    ivk 3
    pop 1

    // set cycle time to tuple
    imp std-tuple:set/3
	llv cycle_time
    lsi 1
    llv msg_tuple
    ivk 3
    pop 1

    // set command to tuple
    imp std-tuple:set/3
    llv msg
    lsi 2
    llv msg_tuple
    ivk 3
    pop 1

    // set serialize data to tupple
    imp std-tuple:set/3
    imp can_db:serialize/1
    llv msg
    ivk 1
    lsi 3
    llv msg_tuple
    ivk 3
    pop 1

    // set inteface
    imp std-tuple:set/3
    imp gen_can_mw:open/1
    lsi 1  // 1 = stub
    llv port
    ivk 2
    lsi 4
    llv msg_tuple
    ivk 3
	pop 1
	llv msg_tuple

    ret
end

def get_list_command()
");
            
            #line 72 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
if( _json.Count == 0) {
            
            #line default
            #line hidden
            this.Write("\tvar [lst_cmd]\r\n\timp std-tuple:new/1\r\n\tlsi 0\r\n\tivk 1\r\n\tslv lst_cmd\r\n");
            
            #line 78 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
} else {
            
            #line default
            #line hidden
            
            #line 79 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
StringBuilder variable = new StringBuilder();
            
            #line default
            #line hidden
            
            #line 80 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
variable.Append(" cmd_" + _json[0].id).Append(", tuple_" + _json[0].id); 
            
            #line default
            #line hidden
            
            #line 81 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
for (var i = 1; i < _json.Count; ++i) {
            
            #line default
            #line hidden
            this.Write("    ");
            
            #line 82 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
variable.Append(", cmd_" + _json[i].id).Append(", tuple_" + _json[i].id); 
            
            #line default
            #line hidden
            
            #line 83 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
}
            
            #line default
            #line hidden
            this.Write("\r\n    var [lst_cmd,");
            
            #line 85 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(variable.ToString()));
            
            #line default
            #line hidden
            this.Write("]\r\n\r\n    // new tuple n command\r\n    imp std-tuple:new/1\r\n    lsi ");
            
            #line 89 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(_json.Count));
            
            #line default
            #line hidden
            this.Write("\r\n    ivk 1\r\n    slv lst_cmd\r\n");
            
            #line 92 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
for (var i = 0; i < _json.Count; ++i) {
            
            #line default
            #line hidden
            
            #line 93 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
 var item = _json[i]; 
            
            #line default
            #line hidden
            
            #line 94 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
 var mapItem = ((JToken)_mapOdb).Where(x => x["command_id"].ToString() == item.id.ToString()).First(); 
            
            #line default
            #line hidden
            
            #line 95 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
 if (mapItem == null) continue; 
            
            #line default
            #line hidden
            this.Write("\r\n    // set data for command ");
            
            #line 97 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(i));
            
            #line default
            #line hidden
            this.Write("\r\n    imp ");
            
            #line 98 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(mapItem["module"]));
            
            #line default
            #line hidden
            this.Write(":new/0\r\n    ivk 0\r\n    slv ");
            
            #line 100 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture("cmd_" + _json[i].id));
            
            #line default
            #line hidden
            this.Write("\r\n    ");
            
            #line 101 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
 for (var j = 0; j < item.signals.Count; ++j) {  
            
            #line default
            #line hidden
            this.Write("    ");
            
            #line 102 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
var signal = item.signals[j]; 
            
            #line default
            #line hidden
            this.Write("    ");
            
            #line 103 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
 var signal_func = mapItem["signals"].Where (x => x["name"] == signal.name).First(); 
            
            #line default
            #line hidden
            this.Write("    ");
            
            #line 104 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
 if (signal_func == null) continue; 
            
            #line default
            #line hidden
            this.Write("\r\n    imp ");
            
            #line 106 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(mapItem["module"]));
            
            #line default
            #line hidden
            this.Write(":");
            
            #line 106 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(signal_func["setter"]));
            
            #line default
            #line hidden
            this.Write("\r\n    ldk ");
            
            #line 107 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(signal.value));
            
            #line default
            #line hidden
            this.Write("\r\n    llv ");
            
            #line 108 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture("cmd_" + _json[i].id));
            
            #line default
            #line hidden
            this.Write("\r\n    ivk 2\r\n    pop 1\r\n    ");
            
            #line 111 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
 } 
            
            #line default
            #line hidden
            this.Write("\r\n    // create tuple data\r\n    imp periodic_message:create_msg_tuple/3\r\n\tlsi ");
            
            #line 115 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(item.cycletime));
            
            #line default
            #line hidden
            this.Write("\r\n    ldk \"");
            
            #line 116 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(item.transmitter));
            
            #line default
            #line hidden
            this.Write("\"\r\n    llv ");
            
            #line 117 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture("cmd_" + _json[i].id));
            
            #line default
            #line hidden
            this.Write("\r\n    ivk 3\r\n    slv ");
            
            #line 119 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture("tuple_" + _json[i].id));
            
            #line default
            #line hidden
            this.Write("\r\n\r\n    // set tuple command to list tuple command\r\n    imp std-tuple:set/3\r\n    " +
                    "llv ");
            
            #line 123 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture("tuple_" + _json[i].id));
            
            #line default
            #line hidden
            this.Write("\r\n    lsi ");
            
            #line 124 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(i));
            
            #line default
            #line hidden
            this.Write("\r\n    llv lst_cmd\r\n    ivk 3\r\n    pop 1\r\n");
            
            #line 128 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
}
            
            #line default
            #line hidden
            
            #line 129 "C:\Users\nguye\Documents\GitHub\osp_ide\tools\ide_compiler\SendPeriodic.tt"
}
            
            #line default
            #line hidden
            this.Write("\r\n\tllv lst_cmd\r\n\r\n    ret\r\nend");
            return this.GenerationEnvironment.ToString();
        }
    }
    
    #line default
    #line hidden
    #region Base class
    /// <summary>
    /// Base class for this transformation
    /// </summary>
    [global::System.CodeDom.Compiler.GeneratedCodeAttribute("Microsoft.VisualStudio.TextTemplating", "14.0.0.0")]
    public class SendPeriodicBase
    {
        #region Fields
        private global::System.Text.StringBuilder generationEnvironmentField;
        private global::System.CodeDom.Compiler.CompilerErrorCollection errorsField;
        private global::System.Collections.Generic.List<int> indentLengthsField;
        private string currentIndentField = "";
        private bool endsWithNewline;
        private global::System.Collections.Generic.IDictionary<string, object> sessionField;
        #endregion
        #region Properties
        /// <summary>
        /// The string builder that generation-time code is using to assemble generated output
        /// </summary>
        protected System.Text.StringBuilder GenerationEnvironment
        {
            get
            {
                if ((this.generationEnvironmentField == null))
                {
                    this.generationEnvironmentField = new global::System.Text.StringBuilder();
                }
                return this.generationEnvironmentField;
            }
            set
            {
                this.generationEnvironmentField = value;
            }
        }
        /// <summary>
        /// The error collection for the generation process
        /// </summary>
        public System.CodeDom.Compiler.CompilerErrorCollection Errors
        {
            get
            {
                if ((this.errorsField == null))
                {
                    this.errorsField = new global::System.CodeDom.Compiler.CompilerErrorCollection();
                }
                return this.errorsField;
            }
        }
        /// <summary>
        /// A list of the lengths of each indent that was added with PushIndent
        /// </summary>
        private System.Collections.Generic.List<int> indentLengths
        {
            get
            {
                if ((this.indentLengthsField == null))
                {
                    this.indentLengthsField = new global::System.Collections.Generic.List<int>();
                }
                return this.indentLengthsField;
            }
        }
        /// <summary>
        /// Gets the current indent we use when adding lines to the output
        /// </summary>
        public string CurrentIndent
        {
            get
            {
                return this.currentIndentField;
            }
        }
        /// <summary>
        /// Current transformation session
        /// </summary>
        public virtual global::System.Collections.Generic.IDictionary<string, object> Session
        {
            get
            {
                return this.sessionField;
            }
            set
            {
                this.sessionField = value;
            }
        }
        #endregion
        #region Transform-time helpers
        /// <summary>
        /// Write text directly into the generated output
        /// </summary>
        public void Write(string textToAppend)
        {
            if (string.IsNullOrEmpty(textToAppend))
            {
                return;
            }
            // If we're starting off, or if the previous text ended with a newline,
            // we have to append the current indent first.
            if (((this.GenerationEnvironment.Length == 0) 
                        || this.endsWithNewline))
            {
                this.GenerationEnvironment.Append(this.currentIndentField);
                this.endsWithNewline = false;
            }
            // Check if the current text ends with a newline
            if (textToAppend.EndsWith(global::System.Environment.NewLine, global::System.StringComparison.CurrentCulture))
            {
                this.endsWithNewline = true;
            }
            // This is an optimization. If the current indent is "", then we don't have to do any
            // of the more complex stuff further down.
            if ((this.currentIndentField.Length == 0))
            {
                this.GenerationEnvironment.Append(textToAppend);
                return;
            }
            // Everywhere there is a newline in the text, add an indent after it
            textToAppend = textToAppend.Replace(global::System.Environment.NewLine, (global::System.Environment.NewLine + this.currentIndentField));
            // If the text ends with a newline, then we should strip off the indent added at the very end
            // because the appropriate indent will be added when the next time Write() is called
            if (this.endsWithNewline)
            {
                this.GenerationEnvironment.Append(textToAppend, 0, (textToAppend.Length - this.currentIndentField.Length));
            }
            else
            {
                this.GenerationEnvironment.Append(textToAppend);
            }
        }
        /// <summary>
        /// Write text directly into the generated output
        /// </summary>
        public void WriteLine(string textToAppend)
        {
            this.Write(textToAppend);
            this.GenerationEnvironment.AppendLine();
            this.endsWithNewline = true;
        }
        /// <summary>
        /// Write formatted text directly into the generated output
        /// </summary>
        public void Write(string format, params object[] args)
        {
            this.Write(string.Format(global::System.Globalization.CultureInfo.CurrentCulture, format, args));
        }
        /// <summary>
        /// Write formatted text directly into the generated output
        /// </summary>
        public void WriteLine(string format, params object[] args)
        {
            this.WriteLine(string.Format(global::System.Globalization.CultureInfo.CurrentCulture, format, args));
        }
        /// <summary>
        /// Raise an error
        /// </summary>
        public void Error(string message)
        {
            System.CodeDom.Compiler.CompilerError error = new global::System.CodeDom.Compiler.CompilerError();
            error.ErrorText = message;
            this.Errors.Add(error);
        }
        /// <summary>
        /// Raise a warning
        /// </summary>
        public void Warning(string message)
        {
            System.CodeDom.Compiler.CompilerError error = new global::System.CodeDom.Compiler.CompilerError();
            error.ErrorText = message;
            error.IsWarning = true;
            this.Errors.Add(error);
        }
        /// <summary>
        /// Increase the indent
        /// </summary>
        public void PushIndent(string indent)
        {
            if ((indent == null))
            {
                throw new global::System.ArgumentNullException("indent");
            }
            this.currentIndentField = (this.currentIndentField + indent);
            this.indentLengths.Add(indent.Length);
        }
        /// <summary>
        /// Remove the last indent that was added with PushIndent
        /// </summary>
        public string PopIndent()
        {
            string returnValue = "";
            if ((this.indentLengths.Count > 0))
            {
                int indentLength = this.indentLengths[(this.indentLengths.Count - 1)];
                this.indentLengths.RemoveAt((this.indentLengths.Count - 1));
                if ((indentLength > 0))
                {
                    returnValue = this.currentIndentField.Substring((this.currentIndentField.Length - indentLength));
                    this.currentIndentField = this.currentIndentField.Remove((this.currentIndentField.Length - indentLength));
                }
            }
            return returnValue;
        }
        /// <summary>
        /// Remove any indentation
        /// </summary>
        public void ClearIndent()
        {
            this.indentLengths.Clear();
            this.currentIndentField = "";
        }
        #endregion
        #region ToString Helpers
        /// <summary>
        /// Utility class to produce culture-oriented representation of an object as a string.
        /// </summary>
        public class ToStringInstanceHelper
        {
            private System.IFormatProvider formatProviderField  = global::System.Globalization.CultureInfo.InvariantCulture;
            /// <summary>
            /// Gets or sets format provider to be used by ToStringWithCulture method.
            /// </summary>
            public System.IFormatProvider FormatProvider
            {
                get
                {
                    return this.formatProviderField ;
                }
                set
                {
                    if ((value != null))
                    {
                        this.formatProviderField  = value;
                    }
                }
            }
            /// <summary>
            /// This is called from the compile/run appdomain to convert objects within an expression block to a string
            /// </summary>
            public string ToStringWithCulture(object objectToConvert)
            {
                if ((objectToConvert == null))
                {
                    throw new global::System.ArgumentNullException("objectToConvert");
                }
                System.Type t = objectToConvert.GetType();
                System.Reflection.MethodInfo method = t.GetMethod("ToString", new System.Type[] {
                            typeof(System.IFormatProvider)});
                if ((method == null))
                {
                    return objectToConvert.ToString();
                }
                else
                {
                    return ((string)(method.Invoke(objectToConvert, new object[] {
                                this.formatProviderField })));
                }
            }
        }
        private ToStringInstanceHelper toStringHelperField = new ToStringInstanceHelper();
        /// <summary>
        /// Helper to produce culture-oriented representation of an object as a string
        /// </summary>
        public ToStringInstanceHelper ToStringHelper
        {
            get
            {
                return this.toStringHelperField;
            }
        }
        #endregion
    }
    #endregion
}