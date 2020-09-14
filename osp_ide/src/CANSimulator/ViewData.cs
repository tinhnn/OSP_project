using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WpfApplication3
{
    // command
    public class ViewData : INotifyPropertyChanged
    {
        private ViewCommandProperties _properties = new ViewCommandProperties();

        public ViewCommandProperties properties
        {
            get { return _properties; }
            set
            {
                if (value != _properties)
                {
                    _properties = value;
                    NotifyPropertyChanged(nameof(properties));
                }
            }
        }

        private int rowValue;
        public int row
        {
            get { return rowValue; }
            set
            {
                if (value != rowValue)
                {
                    rowValue = value;
                    NotifyPropertyChanged(nameof(row));
                }
            }
        }

        private bool isSellectedCommandValue;
        public bool isSellectedCommand
        {
            get { return isSellectedCommandValue; }
            set
            {
                if (value != isSellectedCommandValue)
                {
                    isSellectedCommandValue = value;
                    NotifyPropertyChanged(nameof(isSellectedCommand));
                }
            }
        }

        private bool sendStsValue;
        public bool sendSts
        {
            get { return sendStsValue; }
            set
            {
                if (value != sendStsValue)
                {
                    sendStsValue = value;
                    NotifyPropertyChanged(nameof(sendSts));
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string info)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(info));
        }

    }

    // command properties
    public class ViewCommandProperties : INotifyPropertyChanged
    {
        private bool isCreatingValue;
        public bool isCreating
        {
            get { return isCreatingValue; }
            set
            {
                if (value != isCreatingValue)
                {
                    isCreatingValue = value;
                    NotifyPropertyChanged(nameof(isCreating));
                    foreach (var sig in sigList)
                    {
                        sig.isCreating = isCreatingValue;
                    }
                }
            }
        }

        private string sentStatusValue;
        public string sentStatus
        {
            get { return sentStatusValue; }
            set
            {
                if (value != sentStatusValue)
                {
                    sentStatusValue = value;
                    NotifyPropertyChanged(nameof(sentStatus));
                }
            }
        }

        private bool isCycleValue;
        public bool isCycle
        {
            get { return isCycleValue; }
            set
            {
                if (value != isCycleValue)
                {
                    isCycleValue = value;
                    NotifyPropertyChanged(nameof(isCycle));
                    cycle = isCycleValue == true ? 5 : 0 ;
                }
            }
        }

        private int cycleValue;
        public int cycle
        {
            get { return cycleValue; }
            set
            {
                if (value != cycleValue)
                {
                    cycleValue = value;
                    NotifyPropertyChanged(nameof(cycle));
                }
            }
        }

        private string nameValue;
        public string name
        {
            get { return nameValue; }
            set
            {
                if (value != nameValue)
                {
                    nameValue = value;
                    NotifyPropertyChanged(nameof(name));
                }
            }
        }

        private int idValue;
        public int id
        {
            get { return idValue; }
            set
            {
                if (value != idValue)
                {
                    idValue = value;
                    NotifyPropertyChanged(nameof(id));
                }
            }
        }

        private List<String> trigerListValue = new List<string>();
        public List<string> trigerList
        {
            get { return trigerListValue; }
            set
            {
                if (value != trigerListValue)
                {
                    trigerListValue = value;
                    NotifyPropertyChanged(nameof(trigerList));
                }
            }
        }

        private List<String> chanelListValue = new List<string>();
        public List<string> chanelList
        {
            get { return chanelListValue; }
            set
            {
                if (value != chanelListValue)
                {
                    chanelListValue = value;
                    NotifyPropertyChanged(nameof(chanelList));
                }
            }
        }

        private List<String> typeListValue = new List<string>();
        public List<string> typeList
        {
            get { return typeListValue; }
            set
            {
                if (value != typeListValue)
                {
                    typeListValue = value;
                    NotifyPropertyChanged(nameof(typeList));
                }
            }
        }

        private bool ideValue;
        public bool ide
        {
            get { return ideValue; }
            set
            {
                if (value != ideValue)
                {
                    ideValue = value;
                    NotifyPropertyChanged(nameof(ide));
                }
            }
        }

        private bool brsValue;
        public bool brs
        {
            get { return brsValue; }
            set
            {
                if (value != brsValue)
                {
                    brsValue = value;
                    NotifyPropertyChanged(nameof(brs));
                }
            }
        }

        private bool btrValue;
        public bool btr
        {
            get { return btrValue; }
            set
            {
                if (value != btrValue)
                {
                    btrValue = value;
                    NotifyPropertyChanged(nameof(btr));
                }
            }
        }

        private int trigerSelectedIdxValue;
        public int trigerSelectedIdx
        {
            get { return trigerSelectedIdxValue; }
            set
            {
                if (value != trigerSelectedIdxValue)
                {
                    trigerSelectedIdxValue = value;
                    NotifyPropertyChanged(nameof(trigerSelectedIdx));
                    if (trigerSelectedIdxValue == 0)
                    {
                        isCycle = false;
                        cycle = 0;
                    }
                    else
                    {
                        isCycle = true;
                    }

                }
            }
        }

        private int chanelSelectedIdxValue;
        public int chanelSelectedIdx
        {
            get { return chanelSelectedIdxValue; }
            set
            {
                if (value != chanelSelectedIdxValue)
                {
                    chanelSelectedIdxValue = value;
                    NotifyPropertyChanged(nameof(chanelSelectedIdx));
                }
            }
        }

        private int typeSelectedIdxValue;
        public int typeSelectedIdx
        {
            get { return typeSelectedIdxValue; }
            set
            {
                if (value != typeSelectedIdxValue)
                {
                    typeSelectedIdxValue = value;
                    NotifyPropertyChanged(nameof(typeSelectedIdx));
                }
            }
        }

        private int dlcValue;
        public int dlc
        {
            get { return dlcValue; }
            set
            {
                if (value != dlcValue)
                {
                    dlcValue = value;
                    NotifyPropertyChanged(nameof(dlc));
                }
            }
        }

        private List<ViewSignalProperties> sigListValue = new List<ViewSignalProperties>();
        public List<ViewSignalProperties> sigList
        {
            get { return sigListValue; }
            set
            {
                if (value != sigListValue)
                {
                    sigListValue = value;
                    NotifyPropertyChanged(nameof(sigList));
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }
    }

    // signal properties
    public class ViewSignalProperties : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string info)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(info));
        }

        private bool isSellectedSigValue;
        public bool isSellectedSig
        {
            get { return isSellectedSigValue; }
            set
            {
                if (value != isSellectedSigValue)
                {
                    isSellectedSigValue = value;
                    NotifyPropertyChanged(nameof(isSellectedSig));
                }
            }
        }

        private int rawStepValue;
        public int rawStep
        {
            get { return rawStepValue; }
            set
            {
                if (value != rawStepValue)
                {
                    rawStepValue = value;
                    NotifyPropertyChanged(nameof(rawStep));
                }
            }
        }

        private int physStepValue;
        public int physStep
        {
            get { return physStepValue; }
            set
            {
                if (value != physStepValue)
                {
                    physStepValue = value;
                    NotifyPropertyChanged(nameof(physStep));
                }
            }
        }

        private int rawValValue;
        public int rawVal
        {
            get { return rawValValue; }
            set
            {
                if (value != rawValValue)
                {
                    rawValValue = value;
                    NotifyPropertyChanged(nameof(rawVal));
                }
            }
        }

        private int startBitValue;
        public int startBit
        {
            get { return startBitValue; }
            set
            {
                if (value != startBitValue)
                {
                    startBitValue = value;
                    NotifyPropertyChanged(nameof(startBit));
                }
            }
        }

        private string nameValue;
        public string name
        {
            get { return nameValue; }
            set
            {
                if (value != nameValue)
                {
                    nameValue = value;
                    NotifyPropertyChanged(nameof(name));
                }
            }
        }

        private int lengthValue;
        public int length
        {
            get { return lengthValue; }
            set
            {
                if (value != lengthValue)
                {
                    lengthValue = value;
                    NotifyPropertyChanged(nameof(length));
                }
            }
        }

        private int valSelectedIndexValue;
        public int valSelectedIndex
        {
            get { return valSelectedIndexValue; }
            set
            {
                if (value != valSelectedIndexValue)
                {
                    valSelectedIndexValue = value;
                    NotifyPropertyChanged(nameof(valSelectedIndex));
                }
            }
        }

        private List<int> listRawValValue = new List<int>();
        public List<int> listRawVal
        {
            get { return listRawValValue; }
            set
            {
                if (value != listRawValValue)
                {
                    listRawValValue = value;
                    NotifyPropertyChanged(nameof(listRawVal));
                }
            }
        }

        private List<string> listPhysValValue = new List<string>();
        public List<string> listPhysVal
        {
            get { return listPhysValValue; }
            set
            {
                if (value != listPhysValValue)
                {
                    listPhysValValue = value;
                    NotifyPropertyChanged(nameof(listPhysVal));
                }
            }
        }

        //private List<ViewSignalValue> listValValue = new List<ViewSignalValue>();
        //public List<ViewSignalValue> listVal
        //{
        //    get { return listValValue; }
        //    set
        //    {
        //        if (value != listValValue)
        //        {
        //            listValValue = value;
        //            NotifyPropertyChanged(nameof(listVal));
        //        }
        //    }
        //}

        private bool isCreatingValue;
        public bool isCreating
        {
            get { return isCreatingValue; }
            set
            {
                if (value != isCreatingValue)
                {
                    isCreatingValue = value;
                    NotifyPropertyChanged(nameof(isCreating));
                }
            }
        }

        private int minRawValValue;
        public int minRawVal
        {
            get { return minRawValValue; }
            set
            {
                if (value != minRawValValue)
                {
                    minRawValValue = value;
                    NotifyPropertyChanged(nameof(minRawVal));
                }
            }
        }

        private int maxRawValValue;
        public int maxRawVal
        {
            get { return maxRawValValue; }
            set
            {
                if (value != maxRawValValue)
                {
                    maxRawValValue = value;
                    NotifyPropertyChanged(nameof(maxRawVal));
                }
            }
        }
    }

    public class ViewSignalValue : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(string info)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(info));
        }

        private int rawValValue;
        public int rawVal
        {
            get { return rawValValue; }
            set
            {
                if (value != rawValValue)
                {
                    rawValValue = value;
                    NotifyPropertyChanged(nameof(rawVal));
                }
            }
        }

        private string phyValValue;
        public string phyVal
        {
            get { return phyValValue; }
            set
            {
                if (value != phyValValue)
                {
                    phyValValue = value;
                    NotifyPropertyChanged(nameof(phyVal));
                }
            }
        }

        private string comValValue;
        public string comVal
        {
            get { return comValValue; }
            set
            {
                if (value != comValValue)
                {
                    comValValue = value;
                    NotifyPropertyChanged(nameof(comVal));
                }
            }
        }

    }

}
