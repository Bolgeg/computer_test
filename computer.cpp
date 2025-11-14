class Computer
{
	public:
	
	class Input
	{
		public:
		
		enum class Type{computerInput,computerMemory,nandGate,constant};
		
		Type type=Type::constant;
		int index=0;
		
		Input(){}
		Input(const Type& _type,int _index)
		{
			type=_type;
			index=_index;
		}
		
		bool checkValidity(int numberOfInputs,int numberOfNandGates,int numberOfMemoryBits,string& trace) const
		{
			if(index<0)
			{
				trace+="Error: input: index("+std::to_string(index)+") <0\n";
				return false;
			}
			
			if(type==Type::constant)
			{
				if(index>=2)
				{
					trace+="Error: input: constant value("+std::to_string(index)+") >=2\n";
					return false;
				}
			}
			else if(type==Type::computerInput)
			{
				if(index>=numberOfInputs)
				{
					trace+="Error: input: computer input index("+std::to_string(index)+") >="+std::to_string(numberOfInputs)+"\n";
					return false;
				}
			}
			else if(type==Type::computerMemory)
			{
				if(index>=numberOfMemoryBits)
				{
					trace+="Error: input: computer memory index("+std::to_string(index)+") >="+std::to_string(numberOfMemoryBits)+"\n";
					return false;
				}
			}
			else if(type==Type::nandGate)
			{
				if(index>=numberOfNandGates)
				{
					trace+="Error: input: nand gate index("+std::to_string(index)+") >="+std::to_string(numberOfNandGates)+"\n";
					return false;
				}
			}
			else
			{
				trace+="Error: input: invalid type\n";
				return false;
			}
			return true;
		}
	};
	
	class OutputGate
	{
		public:
		
		Input input;
		
		OutputGate(){}
		explicit OutputGate(const Input& _input)
		{
			input=_input;
		}
		
		bool checkValidity(int numberOfInputs,int numberOfNandGates,int numberOfMemoryBits,string& trace) const
		{
			return input.checkValidity(numberOfInputs,numberOfNandGates,numberOfMemoryBits,trace);
		}
	};
	
	class NandGate
	{
		public:
		
		Input inputA;
		Input inputB;
		
		NandGate(){}
		NandGate(const Input& _inputA,const Input& _inputB)
		{
			inputA=_inputA;
			inputB=_inputB;
		}
		
		bool checkValidity(int numberOfInputs,int numberOfNandGates,int numberOfMemoryBits,string& trace) const
		{
			bool a=inputA.checkValidity(numberOfInputs,numberOfNandGates,numberOfMemoryBits,trace);
			if(!a)
			{
				trace+="(inputA)";
				return false;
			}
			bool b=inputB.checkValidity(numberOfInputs,numberOfNandGates,numberOfMemoryBits,trace);
			if(!b)
			{
				trace+="(inputB)";
				return false;
			}
			return true;
		}
	};
	
	int numberOfInputs=0;
	vector<NandGate> nandGates;
	vector<OutputGate> memory;
	vector<OutputGate> outputs;
	
	int addInputs(int count)
	{
		int index=numberOfInputs;
		numberOfInputs+=count;
		return index;
	}
	int addInput()
	{
		return addInputs(1);
	}
	int addNandGate(const NandGate& nandGate)
	{
		int index=nandGates.size();
		nandGates.emplace_back(nandGate);
		return index;
	}
	int addMemory(const Input& memoryOutputGateInput)
	{
		int index=memory.size();
		memory.emplace_back(memoryOutputGateInput);
		return index;
	}
	int addOutput(const Input& outputOutputGateInput)
	{
		int index=outputs.size();
		outputs.emplace_back(outputOutputGateInput);
		return index;
	}
	
	bool checkValidity(string& trace) const
	{
		trace+="Starting...\n";
		if(numberOfInputs<0) return false;
		
		trace+="Checking nand gates...\n";
		for(int i=0;i<nandGates.size();i++)
		{
			if(!nandGates[i].checkValidity(numberOfInputs,i,memory.size(),trace)) return false;
		}
		
		trace+="Checking memory...\n";
		for(int i=0;i<memory.size();i++)
		{
			if(!memory[i].checkValidity(numberOfInputs,nandGates.size(),memory.size(),trace)) return false;
		}
		
		trace+="Checking outputs...\n";
		for(int i=0;i<outputs.size();i++)
		{
			if(!outputs[i].checkValidity(numberOfInputs,nandGates.size(),memory.size(),trace)) return false;
		}
		
		return true;
	}
	bool checkValidity() const
	{
		string trace;
		return checkValidity(trace);
	}
	
	template <class T>
	class State
	{
		public:
		
		size_t size=1;
		
		vector<T> inputs;
		vector<T> memory;
		vector<T> outputs;
		
		State(){}
		State(Computer& computer,size_t _size)
		{
			*this=State(computer.numberOfInputs,computer.memory.size(),computer.outputs.size(),size);
		}
		State(int numberOfInputs,int numberOfMemoryBits,int numberOfOutputs,size_t _size)
		{
			if constexpr(std::is_same_v<T,vector<uint64_t>>){}
			else
			{
				if(_size>sizeof(T)*8)
				{
					throw string()+"The computer state size is too big for the type: "+std::to_string(_size)+">"+std::to_string(sizeof(T)*8);
				}
			}
			
			size=_size;
			
			inputs=vector<T>(numberOfInputs,getConstant(0));
			memory=vector<T>(numberOfMemoryBits,getConstant(0));
			outputs=vector<T>(numberOfOutputs,getConstant(0));
		}
		
		State getNewEmptyState() const
		{
			return State(inputs.size(),memory.size(),outputs.size(),size);
		}
		
		int getBitValue(const T& bit,size_t index) const
		{
			if constexpr(std::is_same_v<T,vector<uint64_t>>)
			{
				if(index>=size) return 0;
				size_t indexInVector=index/64;
				size_t offset=index%64;
				if(indexInVector>=bit.size()) return 0;
				else return (bit[indexInVector]>>offset)&1;
			}
			else
			{
				if(index>=sizeof(bit)*8) return 0;
				else return (uint64_t(bit)>>index)&1;
			}
		}
		T getConstant(int constant) const
		{
			if constexpr(std::is_same_v<T,vector<uint64_t>>)
			{
				return T(size/64+(size%64!=0 ? 1 : 0),constant ? uint64_t(int64_t(int32_t(-1))) : 0);
			}
			else
			{
				if(constant)
				{
					if constexpr(std::is_same_v<T,uint8_t>) return -1;
					else if constexpr(std::is_same_v<T,uint16_t>) return -1;
					else if constexpr(std::is_same_v<T,uint32_t>) return -1;
					else if constexpr(std::is_same_v<T,uint64_t>) return uint64_t(int64_t(int32_t(-1)));
					else return -1;
				}
				else return 0;
			}
		}
		T computeNand(const T& a,const T& b) const
		{
			if constexpr(std::is_same_v<T,vector<uint64_t>>)
			{
				T r(size/64+(size%64!=0 ? 1 : 0));
				for(size_t i=0;i<r.size();i++)
				{
					r[i]=~(a[i]&b[i]);
				}
				return r;
			}
			else return ~(a&b);
		}
		T getInput(const Input& input,const vector<T>& nandGateOutputs) const
		{
			if(input.type==Input::Type::constant)
			{
				return getConstant(input.index);
			}
			else if(input.type==Input::Type::computerInput) return inputs[input.index];
			else if(input.type==Input::Type::computerMemory) return memory[input.index];
			else if(input.type==Input::Type::nandGate) return nandGateOutputs[input.index];
			else return getConstant(0);
		}
	};
	
	template <class T>
	T getInitialState(size_t size=1)
	{
		return T(*this,size);
	}
	
	template <class T>
	State<T> simulateStep(const State<T>& inputState)
	{
		State<T> outputState=inputState.getNewEmptyState();
		
		vector<T> nandGateOutputs(nandGates.size(),inputState.getConstant(0));
		
		for(int i=0;i<nandGates.size();i++)
		{
			T a=inputState.getInput(nandGates[i].inputA,nandGateOutputs);
			T b=inputState.getInput(nandGates[i].inputB,nandGateOutputs);
			
			nandGateOutputs[i]=inputState.computeNand(a,b);
		}
		
		for(int i=0;i<memory.size();i++)
		{
			outputState.memory[i]=inputState.getInput(memory[i].input,nandGateOutputs);
		}
		
		for(int i=0;i<outputs.size();i++)
		{
			outputState.outputs[i]=inputState.getInput(outputs[i].input,nandGateOutputs);
		}
		
		return outputState;
	}
};
