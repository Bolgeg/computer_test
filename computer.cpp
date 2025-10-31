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
	
	class State
	{
		public:
		
		typedef uint64_t BitType;
		static constexpr int bitDepth=sizeof(BitType)*8;
		
		vector<BitType> inputs;
		vector<BitType> memory;
		vector<BitType> outputs;
		
		State(){}
		State(int numberOfInputs,int numberOfMemoryBits,int numberOfOutputs)
		{
			inputs=vector<BitType>(numberOfInputs,0);
			memory=vector<BitType>(numberOfMemoryBits,0);
			outputs=vector<BitType>(numberOfOutputs,0);
		}
	};
	
	State getInitialState()
	{
		return State(numberOfInputs,memory.size(),outputs.size());
	}
	
	private:
		State::BitType getInput(const Input& input,const State& inputState,const vector<State::BitType>& nandGateOutputs)
		{
			if(input.type==Input::Type::constant)
			{
				if(input.index) return State::BitType(int64_t(-1));
				else return 0;
			}
			else if(input.type==Input::Type::computerInput) return inputState.inputs[input.index];
			else if(input.type==Input::Type::computerMemory) return inputState.memory[input.index];
			else if(input.type==Input::Type::nandGate) return nandGateOutputs[input.index];
			else return 0;
		}
	public:
	State simulateStep(const State& inputState)
	{
		State outputState(numberOfInputs,memory.size(),outputs.size());
		
		vector<State::BitType> nandGateOutputs(nandGates.size(),0);
		
		for(int i=0;i<nandGates.size();i++)
		{
			State::BitType a=getInput(nandGates[i].inputA,inputState,nandGateOutputs);
			State::BitType b=getInput(nandGates[i].inputB,inputState,nandGateOutputs);
			
			nandGateOutputs[i]=~(a&b);
		}
		
		for(int i=0;i<memory.size();i++)
		{
			outputState.memory[i]=getInput(memory[i].input,inputState,nandGateOutputs);
		}
		
		for(int i=0;i<outputs.size();i++)
		{
			outputState.outputs[i]=getInput(outputs[i].input,inputState,nandGateOutputs);
		}
		
		return outputState;
	}
};
