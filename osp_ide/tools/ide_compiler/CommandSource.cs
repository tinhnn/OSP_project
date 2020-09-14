
using System.Security.Cryptography;
using System.Text;

namespace ide_compiler
{
    public partial class SendOneShoot
    {
        private readonly dynamic _json;
        private readonly dynamic _mapOdb;

        public SendOneShoot(dynamic lstOneShoot, dynamic mapOdb)
        {
            _json = lstOneShoot;
            _mapOdb = mapOdb;
        }
    }

    public partial class SendPeriodic
    {
        private readonly dynamic _json;
        private readonly dynamic _mapOdb;

        public SendPeriodic(dynamic lstOneShoot, dynamic mapOdb)
        {
            _json = lstOneShoot;
            _mapOdb = mapOdb;
        }

        public static byte[] GetHash(string inputString)
        {
            HashAlgorithm algorithm = MD5.Create();  //or use SHA256.Create();
            return algorithm.ComputeHash(Encoding.UTF8.GetBytes(inputString));
        }

        public static string GetHashString(string inputString)
        {
            StringBuilder sb = new StringBuilder();
            foreach (byte b in GetHash(inputString))
                sb.Append(b.ToString("X2"));

            return sb.ToString();
        }

    }
}