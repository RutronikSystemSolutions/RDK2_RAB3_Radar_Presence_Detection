using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RDK2_Visualisation_GUI
{
    public class RDK2Event
    {
        public const int PacketLength = 13;
        public float range;
        public float magnitude;
        public float angle;

         /// <summary>
         /// Private constructor
         /// </summary>
        private RDK2Event() { }

        public static RDK2Event? Create(byte[] buffer)
        {
            if (buffer == null) return null;
            if (buffer.Length != PacketLength) return null;
            if (buffer[0] != 0x55) return null;

            RDK2Event retval = new RDK2Event();

            retval.magnitude = BitConverter.ToSingle(buffer, 1);
            retval.range = BitConverter.ToSingle(buffer, 5);
            retval.angle = BitConverter.ToSingle(buffer, 9);

            return retval;
        }
    }
}
