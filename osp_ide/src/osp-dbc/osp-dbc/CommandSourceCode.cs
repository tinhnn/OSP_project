namespace osp.dbc
{
    public partial class CommandSource
    {
        private readonly dynamic _json;

        public CommandSource(dynamic json)
        {
            _json = json;
        }

        public string check_value_type(object value_type)
        {
            if (value_type.ToString() == "signed" || value_type.ToString() == "unsigned")
            {
                return "integer";
            }
            else
            {
                return "real";
            }
        }
    }
}
