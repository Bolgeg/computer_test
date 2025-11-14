class ComputerBuilder
{
	private:
		template <class T>
		class NameMapper
		{
			private:
				std::map<string,T> map;
				T valueToReturnIfNotFound;
			public:
			
			NameMapper(){}
			NameMapper(const T& _valueToReturnIfNotFound)
			{
				valueToReturnIfNotFound=_valueToReturnIfNotFound;
			}
			
			void clear()
			{
				map.clear();
			}
			
			inline T& operator [](const string& name)
			{
				if(!map.contains(name)) return valueToReturnIfNotFound;
				return map[name];
			}
			
			bool containsName(const string& name) const
			{
				return map.contains(name);
			}
			void add(const string& name,const T& value)
			{
				map[name]=value;
			}
			void remove(const string& name)
			{
				map.erase(name);
			}
			void set(const string& name,const T& value)
			{
				map[name]=value;
			}
		};
		
		template <class T>
		class NamedVector
		{
			public:
			
			static constexpr size_t npos=size_t(int64_t(int32_t(-1)));
			
			private:
				using ContainerT=vector<T>;
				using const_iterator=ContainerT::const_iterator;
				
				ContainerT container;
				
				NameMapper<size_t> nameToIndex=NameMapper<size_t>(npos);
				
				void addName(const string& name,size_t index)
				{
					nameToIndex.add(name,index);
				}
				void removeName(const string& name)
				{
					nameToIndex.remove(name);
				}
				void setName(const string& name,size_t index)
				{
					nameToIndex.set(name,index);
				}
				void removeNamesInRange(size_t start,size_t end_)
				{
					for(size_t index=start;index<end_;index++)
					{
						nameToIndex.remove(container[index].name);
					}
					for(size_t index=end_;index<container.size();index++)
					{
						nameToIndex.set(container[index].name,index-(end_-start));
					}
				}
				void addNamesInRange(size_t start,size_t end_)
				{
					for(size_t index=end_;index<container.size();index++)
					{
						nameToIndex.set(container[index].name,index+(end_-start));
					}
					for(size_t index=start;index<end_;index++)
					{
						nameToIndex.add(container[index].name,index);
					}
				}
				void addAllNames()
				{
					addNamesInRange(0,container.size());
				}
			public:
			
			NamedVector(){}
			inline NamedVector(std::initializer_list<T> init)
			{
				container=ContainerT(init);
				
				addAllNames();
			}
			template <class InputIt>
			NamedVector(InputIt first,InputIt last)
			{
				container=ContainerT(first,last);
				
				addAllNames();
			}
			
			inline const T& at(size_t index) const
			{
				return container.at(index);
			}
			inline const T& operator [](size_t index) const
			{
				return container[index];
			}
			inline const T& front() const
			{
				return container.front();
			}
			inline const T& back() const
			{
				return container.back();
			}
			inline const T*data() const
			{
				return container.data();
			}
			
			inline size_t size() const
			{
				return container.size();
			}
			
			void clear()
			{
				container.clear();
				nameToIndex.clear();
			}
			
			const_iterator begin() const
			{
				return container.begin();
			}
			const_iterator end() const
			{
				return container.end();
			}
			
			void insert(const_iterator pos,const T& value)
			{
				size_t position=pos-container.begin();
				container.insert(pos,value);
				
				addNamesInRange(position,position+1);
			}
			template <class InputIt>
			void insert(const_iterator pos,InputIt first,InputIt last)
			{
				size_t position=pos-container.begin();
				size_t length=last-first;
				container.insert(pos,first,last);
				
				addNamesInRange(position,position+length);
			}
			void erase(const_iterator pos)
			{
				size_t position=pos-container.begin();
				removeNamesInRange(position,position+1);
				
				container.erase(pos);
			}
			void erase(const_iterator first,const_iterator last)
			{
				size_t position=first-container.begin();
				size_t length=last-first;
				removeNamesInRange(position,position+length);
				
				container.erase(first,last);
			}
			
			void push_back(const T& value)
			{
				size_t index=container.size();
				
				container.push_back(value);
				
				addName(container[index].name,index);
			}
			void push_back(T&& value)
			{
				size_t index=container.size();
				
				container.push_back(value);
				
				addName(container[index].name,index);
			}
			template <class... Args>
			inline void emplace_back(Args&&... args)
			{
				size_t index=container.size();
				
				container.emplace_back(args...);
				
				addName(container[index].name,index);
			}
			
			void set(size_t index,const T& value)
			{
				string oldName=container.at(index).name;
				
				container[index]=value;
				
				if(container[index].name!=oldName)
				{
					removeName(oldName);
					addName(container[index].name,index);
				}
			}
			size_t find(const string& name)
			{
				return nameToIndex[name];
			}
		};
	public:
	
	class OptimizationOptions
	{
		public:
		
		bool silent=false;
		bool verbose=false;
		bool failAtWarning=false;
		bool optimizeGates=false;
		bool optimizeMemory=false;
		int passes=-1;
		int maxCombinations=1000000;
		double maxTime=-1;
		
		void setAsSilentOptimizeGates(bool _failAtWarning,int _maxCombinations)
		{
			silent=true;
			failAtWarning=_failAtWarning;
			optimizeGates=true;
			maxCombinations=_maxCombinations;
		}
	};
	
	class OptimizationSearchOptions
	{
		public:
		
		bool silent=false;
		string algorithm;
		int maxGates=-1;
		double maxTime=-1;
		
		void setAsSilent(const string& _algorithm,int _maxGates)
		{
			silent=true;
			algorithm=_algorithm;
			maxGates=_maxGates;
		}
	};
	
	class MessageQueue
	{
		private:
			class TypeQueue
			{
				public:
				
				string name;
				
				bool isWarning=false;
				
				uint64_t totalMessageCount=0;
				
				vector<string> messages;
				
				static constexpr int maxSize=4;
				
				TypeQueue(){}
				TypeQueue(const string& _name)
				{
					name=_name;
				}
				
				void add(const string& message,bool _isWarning)
				{
					totalMessageCount++;
					if(_isWarning) isWarning=true;
					if(messages.size()<maxSize) messages.push_back(message);
					else
					{
						for(int i=0;i<maxSize;i++)
						{
							for(int j=0;j<i;j++)
							{
								if(messages[j]==messages[i])
								{
									messages[i]=message;
									return;
								}
							}
						}
					}
				}
				string flushGetString(const string& prefixText)
				{
					string str;
					if(totalMessageCount>0)
					{
						string warningString= isWarning ? "Warning: " : string();
						
						if(totalMessageCount==1)
						{
							str+=prefixText+warningString+messages[0];
						}
						else
						{
							str+=prefixText+warningString+name+" (x"+std::to_string(totalMessageCount)+"):";
							for(int i=0;i<messages.size();i++)
							{
								str+=string()+"\n    '"+messages[i]+"'...";
							}
						}
						
						totalMessageCount=0;
						messages.resize(0);
					}
					return str;
				}
			};
			
			vector<TypeQueue> typeQueues;
			
			string prefixText;
			
			uint64_t messagesPerFlush=1;
			
			string flushTypeGetString(int index)
			{
				string str;
				if(index>=0 && index<typeQueues.size())
				{
					str=typeQueues[index].flushGetString(prefixText);
					typeQueues.erase(typeQueues.begin()+index);
				}
				return str;
			}
			void flushType(int index)
			{
				string str=flushTypeGetString(index);
				if(outputEnabled) std::cout<<str<<std::endl;
			}
			
			bool outputEnabled=true;
			bool throwAtWarning=false;
		public:
		
		MessageQueue(const string& _prefixText=string(),uint64_t _messagesPerFlush=1)
		{
			prefixText=_prefixText;
			messagesPerFlush=_messagesPerFlush;
		}
		void setPrefixText(const string& _prefixText)
		{
			prefixText=_prefixText;
		}
		void setMessagesPerFlush(uint64_t _messagesPerFlush)
		{
			messagesPerFlush=_messagesPerFlush;
		}
		void disableOutput()
		{
			outputEnabled=false;
		}
		void enableOutput()
		{
			outputEnabled=true;
		}
		void setThrowAtWarning(bool value)
		{
			throwAtWarning=value;
		}
		void add(const string& messageType,bool isWarning,const string& message)
		{
			int index=-1;
			for(int i=0;i<typeQueues.size();i++)
			{
				if(typeQueues[i].name==messageType)
				{
					index=i;
					break;
				}
			}
			if(index==-1)
			{
				index=typeQueues.size();
				typeQueues.emplace_back(messageType);
			}
			
			typeQueues[index].add(message,isWarning);
			
			if(isWarning && throwAtWarning)
			{
				throw flushGetString();
			}
			else if(typeQueues[index].totalMessageCount>=messagesPerFlush)
			{
				flushType(index);
			}
		}
		string flushGetString()
		{
			string str;
			for(int i=int(typeQueues.size())-1;i>=0;i--)
			{
				str+=flushTypeGetString(i);
			}
			return str;
		}
		void flush()
		{
			for(int i=int(typeQueues.size())-1;i>=0;i--)
			{
				flushType(i);
			}
		}
	};
	
	class ComputerData
	{
		public:
		
		class Pointer
		{
			public:
			
			enum class Type{none,input,output,memory,nandGate,constant,yesGate};
			
			Type type=Type::none;
			int index=0;
			
			constexpr Pointer(){}
			constexpr Pointer(const Type& _type,int _index)
			{
				type=_type;
				index=_index;
			}
			
			bool isNone() const
			{
				return type==Type::none;
			}
			
			string toString() const
			{
				string str;
				if(type==Type::none) str="none";
				else if(type==Type::input) str="input";
				else if(type==Type::output) str="output";
				else if(type==Type::memory) str="memory";
				else if(type==Type::nandGate) str="nandgate";
				else if(type==Type::constant) str="constant";
				else if(type==Type::yesGate) str="yesgate";
				else str="INVALID";
				str+=std::to_string(index);
				return str;
			}
			
			vector<Pointer> getInputs(const ComputerData& computer) const
			{
				vector<Pointer> inputs;
				if(type==Type::none){}
				else if(type==Type::input){}
				else if(type==Type::output) inputs.push_back(computer.outputs[index].input);
				else if(type==Type::memory) inputs.push_back(computer.memory[index].input);
				else if(type==Type::nandGate)
				{
					inputs.push_back(computer.nandGates[index].inputA);
					inputs.push_back(computer.nandGates[index].inputB);
				}
				else if(type==Type::constant){}
				else if(type==Type::yesGate) inputs.push_back(computer.yesGates[index].input);
				return inputs;
			}
			static vector<Pointer> getDistinct(const vector<Pointer>& pointers)
			{
				vector<Pointer> pointersOut=pointers;
				for(int i=0;i<pointersOut.size();i++)
				{
					for(int j=0;j<i;j++)
					{
						if(pointersOut[j]==pointersOut[i])
						{
							pointersOut.erase(pointersOut.begin()+i);
							i--;
							break;
						}
					}
				}
				return pointersOut;
			}
			vector<Pointer> getInputsDistinct(const ComputerData& computer) const
			{
				return getDistinct(getInputs(computer));
			}
			vector<Pointer*> getInputPtrs(ComputerData& computer) const
			{
				vector<Pointer*> inputPtrs;
				if(type==Type::none){}
				else if(type==Type::input){}
				else if(type==Type::output) inputPtrs.push_back(&computer.outputs[index].input);
				else if(type==Type::memory) inputPtrs.push_back(&computer.memory[index].input);
				else if(type==Type::nandGate)
				{
					inputPtrs.push_back(&computer.nandGates[index].inputA);
					inputPtrs.push_back(&computer.nandGates[index].inputB);
				}
				else if(type==Type::constant){}
				else if(type==Type::yesGate) inputPtrs.push_back(&computer.yesGates[index].input);
				return inputPtrs;
			}
			vector<Pointer> findOutputs(const ComputerData& computer) const
			{
				vector<Pointer> outputs;
				
				if(type==Type::input || type==Type::memory || type==Type::nandGate || type==Type::yesGate)
				{
					for(int i=0;i<computer.outputs.size();i++)
					{
						if(computer.outputs[i].input==*this) outputs.emplace_back(Type::output,i);
					}
					for(int i=0;i<computer.memory.size();i++)
					{
						if(computer.memory[i].input==*this) outputs.emplace_back(Type::memory,i);
					}
					for(int i=0;i<computer.nandGates.size();i++)
					{
						if(computer.nandGates[i].inputA==*this) outputs.emplace_back(Type::nandGate,i);
						if(computer.nandGates[i].inputB==*this) outputs.emplace_back(Type::nandGate,i);
					}
					for(int i=0;i<computer.yesGates.size();i++)
					{
						if(computer.yesGates[i].input==*this) outputs.emplace_back(Type::yesGate,i);
					}
				}
				
				return outputs;
			}
			vector<Pointer> findOutputsDistinct(const ComputerData& computer) const
			{
				return getDistinct(findOutputs(computer));
			}
			
			bool operator ==(const Pointer& b) const
			{
				return type==b.type && index==b.index;
			}
			bool operator !=(const Pointer& b) const
			{
				return !(*this==b);
			}
			bool operator <(const Pointer& b) const
			{
				if(type!=b.type) return type<b.type;
				else return index<b.index;
			}
			bool operator >(const Pointer& b) const
			{
				return b<*this;
			}
			bool operator <=(const Pointer& b) const
			{
				return !(*this>b);
			}
			bool operator >=(const Pointer& b) const
			{
				return !(*this<b);
			}
		};
		
		class YesGate
		{
			public:
			
			bool inputResolved=false;
			Pointer input;
			
			void setInput(const Pointer& _input)
			{
				inputResolved=true;
				input=_input;
			}
		};
		
		class OutputGate
		{
			public:
			
			Pointer input;
			
			OutputGate(){}
			explicit OutputGate(const Pointer& _input)
			{
				input=_input;
			}
		};
		
		class NandGate
		{
			public:
			
			Pointer inputA;
			Pointer inputB;
			
			int lineIndexesIndex=-1;
			
			NandGate(){}
			NandGate(const Pointer& _inputA,const Pointer& _inputB)
			{
				inputA=_inputA;
				inputB=_inputB;
			}
		};
		
		int numberOfInputs=0;
		vector<NandGate> nandGates;
		vector<OutputGate> memory;
		vector<OutputGate> outputs;
		vector<YesGate> yesGates;
		
		vector<vector<int>> nandGates_lineIndexes_array;
		
		void removeLineIndexData()
		{
			for(int i=0;i<nandGates.size();i++)
			{
				nandGates[i].lineIndexesIndex=-1;
			}
			nandGates_lineIndexes_array.resize(0);
		}
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
			nandGates.push_back(nandGate);
			return index;
		}
		int addMemory(const Pointer& memoryOutputGateInput)
		{
			int index=memory.size();
			memory.emplace_back(memoryOutputGateInput);
			return index;
		}
		void setMemory(int index,const Pointer& memoryOutputGateInput)
		{
			memory[index]=OutputGate(memoryOutputGateInput);
		}
		int addOutput(const Pointer& outputOutputGateInput)
		{
			int index=outputs.size();
			outputs.emplace_back(outputOutputGateInput);
			return index;
		}
		int addYesGate(const YesGate& yesGate)
		{
			int index=yesGates.size();
			yesGates.push_back(yesGate);
			return index;
		}
		void setYesGateInput(int index,const Pointer& yesGateInput)
		{
			yesGates[index].setInput(yesGateInput);
		}
		
		private:
			Computer::Input getComputerInput(const Pointer& input) const
			{
				Computer::Input::Type type=Computer::Input::Type::computerInput;
				if(input.type==Pointer::Type::input){}
				else if(input.type==Pointer::Type::memory) type=Computer::Input::Type::computerMemory;
				else if(input.type==Pointer::Type::nandGate) type=Computer::Input::Type::nandGate;
				else if(input.type==Pointer::Type::constant) type=Computer::Input::Type::constant;
				else
				{
					throw string("Error: Invalid gate for final computer");
				}
				
				return Computer::Input(type,input.index);
			}
		public:
		Computer getComputer() const
		{
			Computer computer;
			
			computer.addInputs(numberOfInputs);
			for(int i=0;i<nandGates.size();i++)
			{
				computer.addNandGate(Computer::NandGate(getComputerInput(nandGates[i].inputA),getComputerInput(nandGates[i].inputB)));
			}
			for(int i=0;i<memory.size();i++)
			{
				computer.addMemory(getComputerInput(memory[i].input));
			}
			for(int i=0;i<outputs.size();i++)
			{
				computer.addOutput(getComputerInput(outputs[i].input));
			}
			
			if(yesGates.size()>0) throw string("Error: Invalid gate for final computer");
			
			return computer;
		}
	};
	
	private:
		class SafeInteger
		{
			private:
				int internal_value=0;
				
				static void assertDivision(int dividend,int divisor,bool isModulo)
				{
					if(divisor==0 || dividend==int(uint32_t(1)<<31) && divisor==-1)
					{
						string operationString= isModulo ? "modulo" : "division";
						throw string()+operationString+" operation of invalid integers: "+std::to_string(dividend)+" and "+std::to_string(divisor);
					}
				}
				static bool multiplicationOverflows(int a,int b)
				{
					uint64_t absA=std::abs(int64_t(a));
					uint64_t absB=std::abs(int64_t(b));
					
					uint64_t absC=absA*absB;
					
					if((a<0)!=(b<0) && absC==(uint64_t(1)<<31)) return false;
					if(absC>=(uint64_t(1)<<31)) return true;
					return false;
				}
				static bool additionOverflows_positiveAddition_bothPositive(uint64_t absA,uint64_t absB)
				{
					uint64_t c=absA+absB;
					return c>=(uint64_t(1)<<31);
				}
				static bool additionOverflows_positiveAddition_bothNegative(uint64_t absA,uint64_t absB)
				{
					uint64_t c=absA+absB;
					return c>(uint64_t(1)<<31);
				}
				static bool additionOverflows_positiveAddition_aPositive_bNegative(uint64_t absA,uint64_t absB)
				{
					uint64_t c=absA-absB;
					if(c<(uint64_t(1)<<31)) return false;
					if(-c<=(uint64_t(1)<<31)) return false;
					return true;
				}
				static bool additionSubtractionOverflows(int a,int b,bool isSubtraction)
				{
					bool posA= a>=0;
					bool posB= b>=0;
					
					if(isSubtraction) posB=!posB;
					
					uint64_t absA=std::abs(int64_t(a));
					uint64_t absB=std::abs(int64_t(b));
					
					if(posA && posB) return additionOverflows_positiveAddition_bothPositive(absA,absB);
					if(!posA && !posB) return additionOverflows_positiveAddition_bothNegative(absA,absB);
					if(posA && !posB) return additionOverflows_positiveAddition_aPositive_bNegative(absA,absB);
					if(!posA && posB) return additionOverflows_positiveAddition_aPositive_bNegative(absB,absA);
					
					return false;
				}
				static bool additionOverflows(int a,int b)
				{
					return additionSubtractionOverflows(a,b,false);
				}
				static bool subtractionOverflows(int a,int b)
				{
					return additionSubtractionOverflows(a,b,true);
				}
			public:
			
			explicit SafeInteger(int value)
			{
				internal_value=value;
			}
			int getValue() const
			{
				return internal_value;
			}
			
			SafeInteger operator -() const
			{
				int a=internal_value;
				
				if(uint32_t(a)==(uint32_t(1)<<31)) throw string()+"sign change out of range: "+std::to_string(a);
				int r=-a;
				
				return SafeInteger(r);
			}
			SafeInteger operator ~() const
			{
				int a=internal_value;
				return SafeInteger(~a);
			}
			SafeInteger operator !() const
			{
				int a=internal_value;
				return SafeInteger(!a);
			}
			
			SafeInteger pow(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				int r=0;
				if(a==0 && b<=0) throw string()+"0 to the power of "+std::to_string(b);
				
				if(a==1) r=1;
				else if(a==-1) r=(1-int(uint32_t(b)&1)*2);
				else if(a!=0 && b>=0)
				{
					r=1;
					for(int i=0;i<b;i++)
					{
						if(multiplicationOverflows(r,a)) throw string()+"exponentiation result out of range: "+std::to_string(a)+" and "+std::to_string(b);
						r*=a;
					}
				}
				
				return SafeInteger(r);
			}
			SafeInteger operator *(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				if(multiplicationOverflows(a,b)) throw string()+"multiplication result out of range: "+std::to_string(a)+" and "+std::to_string(b);
				int r=a*b;
				
				return SafeInteger(r);
			}
			SafeInteger operator /(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				assertDivision(a,b,false);
				int r=a/b;
				
				return SafeInteger(r);
			}
			SafeInteger operator %(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				assertDivision(a,b,true);
				int r=a%b;
				
				return SafeInteger(r);
			}
			SafeInteger operator +(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				if(additionOverflows(a,b)) throw string()+"addition overflow: "+std::to_string(a)+" and "+std::to_string(b);
				int r=a+b;
				
				return SafeInteger(r);
			}
			SafeInteger operator -(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				if(subtractionOverflows(a,b)) throw string()+"subtraction overflow: "+std::to_string(a)+" and "+std::to_string(b);
				int r=a-b;
				
				return SafeInteger(r);
			}
			SafeInteger operator <<(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				if(b<0) throw string()+"negative bit shift: "+std::to_string(a)+" and "+std::to_string(b);
				int r= b<32 ? (a>>b) : 0;
				
				return SafeInteger(r);
			}
			SafeInteger operator >>(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				if(b<0) throw string()+"negative bit shift: "+std::to_string(a)+" and "+std::to_string(b);
				int r= b<32 ? (a>>b) : (a<0 ? -1 : 0);
				
				return SafeInteger(r);
			}
			SafeInteger shiftRightLogical(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				if(b<0) throw string()+"negative bit shift: "+std::to_string(a)+" and "+std::to_string(b);
				int r= b<32 ? int(uint32_t(a)>>b) : 0;
				
				return SafeInteger(r);
			}
			SafeInteger operator <(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				int r= a<b;
				
				return SafeInteger(r);
			}
			SafeInteger operator <=(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				int r= a<=b;
				
				return SafeInteger(r);
			}
			SafeInteger operator >(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				int r= a>b;
				
				return SafeInteger(r);
			}
			SafeInteger operator >=(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				int r= a>=b;
				
				return SafeInteger(r);
			}
			SafeInteger operator ==(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				int r= a==b;
				
				return SafeInteger(r);
			}
			SafeInteger operator !=(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				int r= a!=b;
				
				return SafeInteger(r);
			}
			SafeInteger operator &(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				int r=uint32_t(a)&uint32_t(b);
				
				return SafeInteger(r);
			}
			SafeInteger operator ^(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				int r=uint32_t(a)^uint32_t(b);
				
				return SafeInteger(r);
			}
			SafeInteger operator |(const SafeInteger& integerB) const
			{
				int a=internal_value;
				int b=integerB.internal_value;
				
				int r=uint32_t(a)|uint32_t(b);
				
				return SafeInteger(r);
			}
			
			SafeInteger& operator *=(const SafeInteger& b)
			{
				*this=*this*b;
				return *this;
			}
			SafeInteger& operator /=(const SafeInteger& b)
			{
				*this=*this/b;
				return *this;
			}
			SafeInteger& operator %=(const SafeInteger& b)
			{
				*this=*this%b;
				return *this;
			}
			SafeInteger& operator +=(const SafeInteger& b)
			{
				*this=*this+b;
				return *this;
			}
			SafeInteger& operator -=(const SafeInteger& b)
			{
				*this=*this-b;
				return *this;
			}
			SafeInteger& operator <<=(const SafeInteger& b)
			{
				*this=(*this<<b);
				return *this;
			}
			SafeInteger& operator >>=(const SafeInteger& b)
			{
				*this=(*this>>b);
				return *this;
			}
			SafeInteger& operator &=(const SafeInteger& b)
			{
				*this=(*this&b);
				return *this;
			}
			SafeInteger& operator ^=(const SafeInteger& b)
			{
				*this=(*this^b);
				return *this;
			}
			SafeInteger& operator |=(const SafeInteger& b)
			{
				*this=(*this|b);
				return *this;
			}
		};
	public:
	
	class Code
	{
		private:
			template <class T>
			class Container
			{
				private:
					vector<T> elementVector;
				public:
				
				Container()
				{
					elementVector=vector<T>(1);
				}
				T& access()
				{
					return elementVector[0];
				}
				T get() const
				{
					return elementVector[0];
				}
			};
			
			class InputExpression;
			
			static constexpr int maximumVariableSizeInBits=(1<<30);
		public:
		
		class Component
		{
			public:
			
			class Variable
			{
				public:
				
				enum class Type{input,output,reg,intermediate};
				
				Type type=Type::intermediate;
				string name;
				int sizeInBits=1;
				
				int lineIndex=-1;
				
				Variable(){}
				Variable(const Type& _type,const string& _name,int _sizeInBits,int _lineIndex)
				{
					type=_type;
					name=_name;
					sizeInBits=_sizeInBits;
					
					lineIndex=_lineIndex;
				}
			};
			
			class InputOrOutputInfo
			{
				public:
				
				string name;
				int sizeInBits=1;
				
				InputOrOutputInfo(){}
				InputOrOutputInfo(const string& _name,int _sizeInBits)
				{
					name=_name;
					sizeInBits=_sizeInBits;
				}
			};
			
			class Element
			{
				public:
				
				class Output
				{
					public:
					
					int variableIndex=-1;
					
					Output(){}
					explicit Output(int _variableIndex)
					{
						variableIndex=_variableIndex;
					}
				};
				
				class Input
				{
					public:
					
					Container<InputExpression> expression;
					
					Input(){}
					Input(const InputExpression& _expression)
					{
						expression.access()=_expression;
					}
				};
				
				enum class Type{nand,set,concat,component};
				
				Type type=Type::nand;
				
				string componentName;
				int componentIndex=-1;
				
				int numberOfInstances=1;
				
				vector<Output> outputs;
				vector<Input> inputs;
				
				int lineIndex=-1;
				
				Element(){}
				Element(const Type& _type,const string& _componentName,int _componentIndex,int _numberOfInstances,const vector<Output>& _outputs,const vector<Input>& _inputs,
					int _lineIndex)
				{
					type=_type;
					componentName=_componentName;
					componentIndex=_componentIndex;
					numberOfInstances=_numberOfInstances;
					outputs=_outputs;
					inputs=_inputs;
					lineIndex=_lineIndex;
				}
			};
			
			string name;
			
			int firstLine=0;
			int lastLine=0;
			
			vector<InputOrOutputInfo> inputs;
			vector<InputOrOutputInfo> outputs;
			
			NamedVector<Variable> variables;
			
			vector<Element> elements;
			
			Component(){}
			explicit Component(const string& _name)
			{
				name=_name;
			}
		};
		
		vector<string> originalLines;
		bool originalLinesProcessed=false;
		vector<int> lineIndexToOriginalLineIndex;
		vector<string> lines;
		
		vector<Component> components;
		
		Code(){}
		explicit Code(const string& codeText)
		{
			string line;
			for(size_t p=0;p<codeText.size();p++)
			{
				uint8_t c=codeText[p];
				
				if(c=='\n' || c=='\r')
				{
					if(p+1<codeText.size())
					{
						uint8_t c2=codeText[p+1];
						if((c2=='\n' || c2=='\r') && c2!=c)
						{
							p++;
						}
					}
					
					addLine(line);
					line=string();
				}
				else
				{
					line.push_back(c);
				}
			}
			addLine(line);
		}
		private:
			void checkInputLineCharactersValid(const string& inputLine,size_t lineIndex)
			{
				for(size_t p=0;p<inputLine.size();p++)
				{
					uint8_t c=inputLine[p];
					if(c<32 || c>126)
					{
						throw string("Code compilation error: Invalid character byte ")+std::to_string(uint32_t(c))+" at line "+std::to_string(lineIndex+1)
							+" at position "+std::to_string(p+1);
					}
				}
			}
			void addLine(const string& inputLine)
			{
				checkInputLineCharactersValid(inputLine,lines.size());
				string outputLine;
				bool ignoreSpaces=true;
				for(size_t p=0;p<inputLine.size();p++)
				{
					uint8_t c=inputLine[p];
					
					if(c==' ' || c=='\t')
					{
						if(!ignoreSpaces)
						{
							outputLine.push_back(' ');
							ignoreSpaces=true;
						}
					}
					else
					{
						ignoreSpaces=false;
						outputLine.push_back(c);
					}
				}
				if(outputLine.size()>0)
				{
					if(outputLine.back()==' ') outputLine.resize(outputLine.size()-1);
				}
				originalLines.push_back(outputLine);
			}
			int resolveLineIndex(int lineIndex)
			{
				if(originalLinesProcessed)
				{
					if(lineIndex>=0 && lineIndex<lineIndexToOriginalLineIndex.size())
					{
						return lineIndexToOriginalLineIndex[lineIndex];
					}
					else
					{
						return 1000000+lineIndex;
					}
				}
				else
				{
					return lineIndex;
				}
			}
			string errorString(const string& errorMessage,int lineIndex)
			{
				int originalLineIndex=resolveLineIndex(lineIndex);
				
				string lineContent;
				if(originalLineIndex>=0 && originalLineIndex<originalLines.size())
				{
					lineContent+=string(":\n")+originalLines[originalLineIndex];
					if(originalLinesProcessed && lineIndex>=0 && lineIndex<lines.size() && lines[lineIndex]!=originalLines[originalLineIndex])
					{
						lineContent+=string("\n|\nv\n")+lines[lineIndex];
					}
				}
				
				return string("Error: ")+errorMessage+"\n"+"at line "+std::to_string(originalLineIndex+1)+lineContent;
			}
			string errorString(int errorCode,int lineIndex)
			{
				return errorString(string("ERROR_")+std::to_string(errorCode),lineIndex);
			}
			
			bool isValidIdentifier(const string& identifier)
			{
				if(identifier.size()==0) return false;
				for(size_t p=0;p<identifier.size();p++)
				{
					uint8_t c=identifier[p];
					if(!(c>='A' && c<='Z' || c>='a' && c<='z' || c=='_'))
					{
						if(p==0) return false;
						else if(!(c>='0' && c<='9')) return false;
					}
				}
				return true;
			}
			
			template<class T>
			static int findWithName(const vector<T>& v,const string& name)
			{
				for(int i=0;i<v.size();i++)
				{
					if(v[i].name==name) return i;
				}
				return -1;
			}
			template<class T>
			static int findWithName(NamedVector<T>& v,const string& name)
			{
				size_t index=v.find(name);
				if(index!=NamedVector<T>::npos) return index;
				return -1;
			}
			static bool stringToIntSimple(const string& str,int& value)
			{
				try
				{
					value=std::stoi(str);
					if(std::to_string(value)!=str) throw 1;
					return true;
				}
				catch(...)
				{
					return false;
				}
			}
			
			vector<string> splitStringAtSpaces(const string& line)
			{
				vector<string> strs;
				
				string str;
				for(size_t p=0;p<line.size();p++)
				{
					if(line[p]==' ')
					{
						if(str.size()>0)
						{
							strs.emplace_back(str);
							str=string();
						}
					}
					else
					{
						str.push_back(line[p]);
					}
				}
				if(str.size()>0) strs.emplace_back(str);
				
				return strs;
			}
			vector<string> splitStringAtCharacterMatches(const string& line,uint8_t character)
			{
				vector<string> strs;
				
				string str;
				for(size_t p=0;p<line.size();p++)
				{
					if(line[p]==character)
					{
						strs.emplace_back(str);
						str=string();
					}
					else
					{
						str.push_back(line[p]);
					}
				}
				strs.emplace_back(str);
				
				return strs;
			}
			vector<string> splitStringAtColons(const string& line)
			{
				return splitStringAtCharacterMatches(line,':');
			}
			vector<string> splitStringAtCommas(const string& line)
			{
				return splitStringAtCharacterMatches(line,',');
			}
			string trimString(const string& line)
			{
				size_t start=line.find_first_not_of(" ");
				if(start==string::npos) return string();
				
				size_t end=line.find_last_not_of(" ");
				
				return line.substr(start,(end+1)-start);
			}
			bool stringStartsWith(const string& str,const string& ref)
			{
				if(str.size()<ref.size()) return false;
				return str.substr(0,ref.size())==ref;
			}
			bool stringEndsWith(const string& str,const string& ref)
			{
				if(str.size()<ref.size()) return false;
				return str.substr(str.size()-ref.size(),ref.size())==ref;
			}
			
			void addTemplateProcessedLine(const string& line,int lineIndex)
			{
				if(lines.size()+1>((uint32_t(1)<<31)-1)) throw errorString("Reached limit of number of lines while processing templates",lineIndex);
				lines.emplace_back(line);
				lineIndexToOriginalLineIndex.push_back(lineIndex);
			}
			vector<string> splitLineTemplates(const string& line)
			{
				vector<string> strs;
				string str;
				int parenthesisLevel=0;
				for(size_t p=0;p<line.size();p++)
				{
					bool insideTemplateExpression= str.size()>0 && str[0]=='<';
					
					bool ignoreTemplateSigns= insideTemplateExpression && parenthesisLevel>0;
					
					uint8_t c=line[p];
					
					if(c=='(') parenthesisLevel++;
					else if(c==')') parenthesisLevel--;
					
					if(c=='<' && !ignoreTemplateSigns)
					{
						if(str.size()>0)
						{
							strs.emplace_back(str);
							str=string();
						}
						str.push_back(c);
						
						parenthesisLevel=0;
					}
					else if(c=='>' && !ignoreTemplateSigns)
					{
						str.push_back(c);
						strs.emplace_back(str);
						str=string();
						
						parenthesisLevel=0;
					}
					else
					{
						str.push_back(c);
					}
				}
				if(str.size()>0) strs.emplace_back(str);
				return strs;
			}
			bool checkSplittedTemplateLineValid(const vector<string>& strs)
			{
				for(size_t i=0;i<strs.size();i++)
				{
					if(strs[i].size()==0) return false;
					else if((strs[i][0]=='<') != (strs[i].back()=='>')) return false;
				}
				return true;
			}
			
			class TemplateParameter
			{
				public:
				
				string name;
				vector<string> possibleValues;
				
				TemplateParameter(){}
				TemplateParameter(const string& _name,const vector<string>& _possibleValues)
				{
					name=_name;
					possibleValues=_possibleValues;
				}
			};
			
			class TemplateArgument
			{
				public:
				
				string name;
				string value;
				
				TemplateArgument(){}
				TemplateArgument(const string& _name,const string& _value)
				{
					name=_name;
					value=_value;
				}
			};
			
			class TokenizedCode
			{
				public:
				
				vector<string> tokens;
				
				TokenizedCode(){}
				explicit TokenizedCode(const string& codeString)
				{
					tokens=tokenize(codeString);
				}
				
				static vector<string> tokenize(const string& codeString)
				{
					vector<string> outputTokens;
					string wordToken;
					for(size_t p=0;p<codeString.size();p++)
					{
						uint8_t c=codeString[p];
						
						if(isCharacterOfWordToken(c))
						{
							wordToken.push_back(c);
						}
						else
						{
							if(wordToken.size()>0)
							{
								outputTokens.push_back(wordToken);
								wordToken=string();
							}
							outputTokens.emplace_back(1,c);
						}
					}
					if(wordToken.size()>0)
					{
						outputTokens.push_back(wordToken);
						wordToken=string();
					}
					return outputTokens;
				}
				static string untokenize(const vector<string>& inputTokens)
				{
					string str;
					for(size_t t=0;t<inputTokens.size();t++)
					{
						str+=inputTokens[t];
					}
					return str;
				}
				
				private:
					static bool isCharacterOfWordToken(uint8_t c)
					{
						return c>='a' && c<='z' || c>='A' && c<='Z' || c>='0' && c<='9' || c=='_';
					}
				public:
			};
			
			template <class ValueT,class ContextT,class CodeT>
			class ExpressionEvaluator
			{
				public:
				
				class EvaluatorContext
				{
					public:
					
					const CodeT*codePtr=nullptr;
					size_t tokenIndex=0;
					int subexpressionLevel=0;
					
					EvaluatorContext(){}
					EvaluatorContext(const CodeT& _code,size_t _tokenIndex,int _subexpressionLevel)
					{
						codePtr=&_code;
						tokenIndex=_tokenIndex;
						subexpressionLevel=_subexpressionLevel;
					}
				};
				
				class Operator
				{
					private:
						using FunctionType=std::function<ValueT(const vector<ValueT>&,ContextT&,const EvaluatorContext&)>;
						using FunctionTypeNoEvaluatorContext=std::function<ValueT(const vector<ValueT>&,ContextT&)>;
						using FunctionTypeNoContext=std::function<ValueT(const vector<ValueT>&)>;
						using CheckFunctionType=std::function<vector<ValueT>(const vector<ValueT>&,ContextT&,const EvaluatorContext&)>;
						
						bool defined=false;
						
						string name;
						
						vector<vector<string>> nameTokens;
						
						FunctionType function;
						
						CheckFunctionType checkFunction;
						
						int numberOfArguments=-1;
						
						bool allowSkippedArguments=false;
						
						int separatorIndex=-1;
						int terminatorIndex=-1;
						
						int operatorLevel=-1;
					public:
					
					bool isDefined() const
					{
						return defined;
					}
					string getName() const
					{
						return name;
					}
					vector<vector<string>> getNameTokens() const
					{
						return nameTokens;
					}
					vector<string> getNameTokensFirst() const
					{
						return nameTokens[0];
					}
					int getSeparatorIndex() const
					{
						return separatorIndex;
					}
					int getTerminatorIndex() const
					{
						return terminatorIndex;
					}
					vector<string> getSeparator() const
					{
						if(separatorIndex==-1) return vector<string>();
						return nameTokens[separatorIndex];
					}
					vector<string> getTerminator() const
					{
						if(terminatorIndex==-1) return vector<string>();
						return nameTokens[terminatorIndex];
					}
					int getOperatorLevel() const
					{
						return operatorLevel;
					}
					bool getAllowSkippedArguments() const
					{
						return allowSkippedArguments;
					}
					void setFixedNumberOfArguments(int _numberOfArguments)
					{
						numberOfArguments=_numberOfArguments;
						validate();
					}
					void setAllowSkippedArguments(bool _allowSkippedArguments)
					{
						allowSkippedArguments=_allowSkippedArguments;
						validate();
					}
					void setAsStarterAndSeparator()
					{
						separatorIndex=nameTokens.size()>=2 ? 1 : -1;
						terminatorIndex=-1;
						validate();
					}
					void setAsStarterAndOptionalSeparatorAndTerminator()
					{
						separatorIndex=nameTokens.size()>=3 ? 1 : -1;
						terminatorIndex=nameTokens.size()>=2 ? nameTokens.size()-1 : -1;
						validate();
					}
					void setOperatorLevel(int _operatorLevel)
					{
						operatorLevel=_operatorLevel;
						validate();
					}
					
					private:
						void validate()
						{
							if(name.size()==0 || nameTokens.size()==0) throw string()+"Internal error: operator without name";
							for(size_t i=0;i<nameTokens.size();i++)
							{
								if(nameTokens[i].size()==0) throw string()+"Internal error: operator with empty string in name";
							}
							if(function==nullptr) throw string()+"Internal error: operator without function";
							if(numberOfArguments<-1) throw string()+"Internal error: operator with invalid number of arguments";
							if(separatorIndex<-1 || separatorIndex>=int(nameTokens.size())) throw string()+"Internal error: operator with invalid separator index";
							if(terminatorIndex<-1 || terminatorIndex>=int(nameTokens.size())) throw string()+"Internal error: operator with invalid terminator index";
							
							defined=true;
						}
						
						template <class NameType,class FT>
						void initialize(const NameType& _name,const FT& _function,const CheckFunctionType& _checkFunction,
							int _numberOfArguments=-1,bool _allowSkippedArguments=false)
						{
							vector<string> nameVector;
							if constexpr(std::is_same_v<NameType,vector<string>>) nameVector=_name;
							else nameVector=vector<string>{string(_name)};
							
							for(size_t i=0;i<nameVector.size();i++)
							{
								nameTokens.push_back(CodeT::tokenize(nameVector[i]));
							}
							name= nameTokens.size()>0 ? CodeT::untokenize(nameTokens[0]) : string();
							
							if constexpr(std::is_same_v<FT,FunctionType>) function=_function;
							else if constexpr(std::is_same_v<FT,FunctionTypeNoEvaluatorContext>)
							{
								function=[_function](const vector<ValueT>& args,ContextT& context,const EvaluatorContext& evaluatorContext)->ValueT
									{
										return _function(args,context);
									};
							}
							else if constexpr(std::is_same_v<FT,FunctionTypeNoContext>)
							{
								function=[_function](const vector<ValueT>& args,ContextT& context,const EvaluatorContext& evaluatorContext)->ValueT
									{
										return _function(args);
									};
							}
							
							checkFunction=_checkFunction;
							
							numberOfArguments=_numberOfArguments;
							
							allowSkippedArguments=_allowSkippedArguments;
							
							validate();
						}
					public:
					
					Operator(){}
					
					template <class NameType>
					Operator(const NameType& _name,const FunctionType& _function,int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,nullptr,_numberOfArguments,_allowSkippedArguments);
					}
					template <class NameType>
					Operator(const NameType& _name,const FunctionTypeNoEvaluatorContext& _function,int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,nullptr,_numberOfArguments,_allowSkippedArguments);
					}
					template <class NameType>
					Operator(const NameType& _name,const FunctionTypeNoContext& _function,int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,nullptr,_numberOfArguments,_allowSkippedArguments);
					}
					
					template <class NameType>
					Operator(const NameType& _name,const FunctionType& _function,const CheckFunctionType& _checkFunction,
						int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,_checkFunction,_numberOfArguments,_allowSkippedArguments);
					}
					template <class NameType>
					Operator(const NameType& _name,const FunctionTypeNoEvaluatorContext& _function,const CheckFunctionType& _checkFunction,
						int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,_checkFunction,_numberOfArguments,_allowSkippedArguments);
					}
					template <class NameType>
					Operator(const NameType& _name,const FunctionTypeNoContext& _function,const CheckFunctionType& _checkFunction,
						int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,_checkFunction,_numberOfArguments,_allowSkippedArguments);
					}
					
					ValueT execute(const vector<ValueT>& args,ContextT& context,const EvaluatorContext& evaluatorContext) const
					{
						if(!isDefined()) throw string()+"Internal error: executing an operator that is not defined";
						if(numberOfArguments!=-1)
						{
							if(args.size()!=numberOfArguments) throw string()+"wrong number of arguments for operator: '"+name+"'";
						}
						return function(args,context,evaluatorContext);
					}
					vector<ValueT> executeCheck(const vector<ValueT>& args,ContextT& context,const EvaluatorContext& evaluatorContext) const
					{
						if(!isDefined()) throw string()+"Internal error: executing check of an operator that is not defined";
						if(checkFunction==nullptr) return args;
						return checkFunction(args,context,evaluatorContext);
					}
				};
				
				private:
					void setFixedNumberOfArguments(vector<Operator>& ops,int numberOfArguments)
					{
						for(int f=0;f<ops.size();f++)
						{
							ops[f].setFixedNumberOfArguments(numberOfArguments);
						}
					}
					void setAsStarterAndSeparator(vector<Operator>& ops)
					{
						for(int f=0;f<ops.size();f++)
						{
							ops[f].setAsStarterAndSeparator();
						}
					}
					void setAsStarterAndOptionalSeparatorAndTerminator(vector<Operator>& ops)
					{
						for(int f=0;f<ops.size();f++)
						{
							ops[f].setAsStarterAndOptionalSeparatorAndTerminator();
						}
					}
					
					vector<Operator> unaryOperators;
					vector<Operator> binaryOperators;
					vector<vector<string>> binaryOperatorLevels;
					vector<Operator> ternaryOperators;
					vector<Operator> bracketFunctionOperators;
					vector<Operator> bracketOperators;
					
					std::function<ValueT(const string&,ContextT&,const EvaluatorContext&)> tokenResolutionFunction;
					
					bool changed=true;
				public:
				
				void setTokenResolutionFunction(const std::function<ValueT(const string&,ContextT&,const EvaluatorContext&)>& f)
				{
					tokenResolutionFunction=f;
					changed=true;
				}
				void setUnaryOperators(const vector<Operator>& ops)
				{
					unaryOperators=ops;
					setFixedNumberOfArguments(unaryOperators,1);
					changed=true;
				}
				void setBinaryOperators(const vector<Operator>& ops,const vector<vector<string>>& operatorLevels)
				{
					binaryOperators=ops;
					setFixedNumberOfArguments(binaryOperators,2);
					
					binaryOperatorLevels=operatorLevels;
					std::reverse(binaryOperatorLevels.begin(),binaryOperatorLevels.end());
					
					for(int i=0;i<binaryOperators.size();i++)
					{
						binaryOperators[i].setOperatorLevel(-1);
					}
					for(int i=0;i<binaryOperatorLevels.size();i++)
					{
						for(int j=0;j<binaryOperatorLevels[i].size();j++)
						{
							string name=CodeT::untokenize(CodeT::tokenize(binaryOperatorLevels[i][j]));
							
							Operator*op=findOperatorWithName(binaryOperators,name);
							if(op!=nullptr) op->setOperatorLevel(getBinaryOperatorLevel(i));
						}
					}
					for(int i=0;i<binaryOperators.size();i++)
					{
						Operator*op=&binaryOperators[i];
						if(op->getOperatorLevel()==-1)
						{
							throw string()+"internal error: binary operator with no defined level: '"+op->getName()+"'";
						}
					}
					
					changed=true;
				}
				void setTernaryOperators(const vector<Operator>& ops)
				{
					ternaryOperators=ops;
					setFixedNumberOfArguments(ternaryOperators,3);
					setAsStarterAndSeparator(ternaryOperators);
					changed=true;
				}
				void setBracketFunctionOperators(const vector<Operator>& ops)
				{
					bracketFunctionOperators=ops;
					setAsStarterAndOptionalSeparatorAndTerminator(bracketFunctionOperators);
					changed=true;
				}
				void setBracketOperators(const vector<Operator>& ops)
				{
					bracketOperators=ops;
					setAsStarterAndOptionalSeparatorAndTerminator(bracketOperators);
					changed=true;
				}
				
				private:
					vector<vector<string>> delimiters;
					
					void addDelimiter(const vector<string>& v)
					{
						if(v.size()>0 && !isDelimiter(v))
						{
							delimiters.push_back(v);
						}
					}
					void updateChanges()
					{
						delimiters=vector<vector<string>>();
						for(int i=0;i<ternaryOperators.size();i++)
						{
							if(!ternaryOperators[i].isDefined()) continue;
							addDelimiter(ternaryOperators[i].getSeparator());
						}
						for(int i=0;i<bracketFunctionOperators.size();i++)
						{
							if(!bracketFunctionOperators[i].isDefined()) continue;
							addDelimiter(bracketFunctionOperators[i].getSeparator());
							addDelimiter(bracketFunctionOperators[i].getTerminator());
						}
						for(int i=0;i<bracketOperators.size();i++)
						{
							if(!bracketOperators[i].isDefined()) continue;
							addDelimiter(bracketOperators[i].getSeparator());
							addDelimiter(bracketOperators[i].getTerminator());
						}
						
						changed=false;
					}
					
					bool isDelimiter(const vector<string>& v)
					{
						for(int i=0;i<delimiters.size();i++)
						{
							if(v==delimiters[i]) return true;
						}
						return false;
					}
					
					static constexpr int ternaryOperatorLevel=1;
					static constexpr int binaryOperatorLevels_start=2;
					static constexpr int unaryOperatorLevel_aboveBinaryOperators=1;
					static constexpr int bracketFunctionOperatorLevel_aboveBinaryOperators=2;
					
					int getTernaryOperatorLevel()
					{
						return ternaryOperatorLevel;
					}
					int getBinaryOperatorLevel(int offset)
					{
						return binaryOperatorLevels_start+offset;
					}
					int getUnaryOperatorLevel()
					{
						return binaryOperatorLevels_start+int(binaryOperatorLevels.size())-1+unaryOperatorLevel_aboveBinaryOperators;
					}
					int getBracketFunctionOperatorLevel()
					{
						return binaryOperatorLevels_start+int(binaryOperatorLevels.size())-1+bracketFunctionOperatorLevel_aboveBinaryOperators;
					}
					
					bool parseOperator(const CodeT& code,size_t& tokenIndex,Operator*& outputOp,vector<Operator>& operators)
					{
						size_t t=tokenIndex;
						vector<int> matches(operators.size(),true);
						int candidateIndex=-1;
						for(int tokens=1;;tokens++)
						{
							if(t>=code.tokens.size()) break;
							
							string token=code.tokens[t];
							
							int matchIndex=-1;
							for(int o=0;o<matches.size();o++)
							{
								if(!matches[o]) continue;
								
								if(!operators[o].isDefined())
								{
									matches[o]=false;
									continue;
								}
								
								if(operators[o].getNameTokensFirst().size()<tokens) matches[o]=false;
								else matches[o]= operators[o].getNameTokensFirst()[tokens-1]==token;
								
								if(matches[o]) matchIndex=o;
								
								if(matches[o] && operators[o].getNameTokensFirst().size()==tokens)
								{
									candidateIndex=o;
								}
							}
							
							if(matchIndex==-1) break;
							
							t++;
						}
						
						if(candidateIndex==-1) return false;
						
						tokenIndex=t;
						outputOp=&operators[candidateIndex];
						return true;
					}
					bool parseOperatorDelimiter(const CodeT& code,size_t& tokenIndex,bool& isSeparator,bool& isTerminator,Operator*op)
					{
						size_t t=tokenIndex;
						vector<vector<string>> nameTokens=op->getNameTokens();
						vector<int> matches(nameTokens.size(),true);
						int candidateIndex=-1;
						for(int tokens=1;;tokens++)
						{
							if(t>=code.tokens.size()) break;
							
							string token=code.tokens[t];
							
							int matchIndex=-1;
							for(int i=0;i<matches.size();i++)
							{
								if(!matches[i]) continue;
								
								if(nameTokens[i].size()<tokens) matches[i]=false;
								else matches[i]= nameTokens[i][tokens-1]==token;
								
								if(matches[i]) matchIndex=i;
								
								if(matches[i] && nameTokens[i].size()==tokens)
								{
									candidateIndex=i;
								}
							}
							
							if(matchIndex==-1) break;
							
							t++;
						}
						
						if(candidateIndex==-1) return false;
						
						isSeparator= candidateIndex==op->getSeparatorIndex();
						isTerminator= candidateIndex==op->getTerminatorIndex();
						
						if(!isSeparator && !isTerminator) return false;
						
						tokenIndex=t;
						return true;
					}
					bool parseCheckDelimiter(const CodeT& code,size_t tokenIndex)
					{
						size_t t=tokenIndex;
						vector<int> matches(delimiters.size(),true);
						int candidateIndex=-1;
						for(int tokens=1;;tokens++)
						{
							if(t>=code.tokens.size()) break;
							
							string token=code.tokens[t];
							
							int matchIndex=-1;
							for(int i=0;i<matches.size();i++)
							{
								if(!matches[i]) continue;
								
								if(delimiters[i].size()<tokens) matches[i]=false;
								else matches[i]= delimiters[i][tokens-1]==token;
								
								if(matches[i]) matchIndex=i;
								
								if(matches[i] && delimiters[i].size()==tokens)
								{
									candidateIndex=i;
								}
							}
							
							if(matchIndex==-1) break;
							
							t++;
						}
						
						return candidateIndex!=-1;
					}
					Operator*findOperatorWithName(vector<Operator>& f,const string& name)
					{
						for(int i=0;i<f.size();i++)
						{
							if(f[i].getName()==name) return &f[i];
						}
						return nullptr;
					}
					
					string expectedSeparatorMessage(Operator*op,const string& actualToken,int errorCode)
					{
						string separator=CodeT::untokenize(op->getSeparator());
						return string()+"expected separator '"+separator+"' instead of '"+actualToken+"'";
					}
					string expectedSeparatorOrTerminatorMessage(Operator*op,const string& actualToken,int errorCode)
					{
						string separator=CodeT::untokenize(op->getSeparator());
						string terminator=CodeT::untokenize(op->getTerminator());
						return string()+"expected separator '"+separator+"' or terminator '"+terminator+"' instead of '"+actualToken+"'";
					}
					string expectedContinuationMessage(int errorCode)
					{
						return "expected continuation of expression";
					}
					string expectedTokenMessage(const string& expectedToken,const string& actualToken,int errorCode)
					{
						return string()+"expected '"+expectedToken+"' instead of '"+actualToken+"'";
					}
					string unexpectedTokenMessage(const string& token,int errorCode)
					{
						return string()+"unexpected symbol '"+token+"'";
					}
					
					ValueT executeVariableArgumentOperatorEvaluatingArguments(const CodeT& code,size_t& t,ContextT& context,
						Operator*op,const vector<ValueT>& startArguments,int subexpressionLevel)
					{
						vector<ValueT> arguments=startArguments;
						
						bool allowSkippedArguments=op->getAllowSkippedArguments();
						
						int argumentsWithoutCheck=0;
						
						for(;;)
						{
							bool emptyArgument=false;
							
							bool isSeparator=false;
							bool isTerminator=false;
							parseOperatorDelimiter(code,t,isSeparator,isTerminator,op);
							
							if(allowSkippedArguments && (isSeparator || isTerminator))
							{
								emptyArgument=true;
							}
							else if(!allowSkippedArguments && isTerminator) break;
							
							if(argumentsWithoutCheck)
							{
								arguments=op->executeCheck(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
								argumentsWithoutCheck=0;
							}
							ValueT argument;
							if(emptyArgument)
							{
								if(tokenResolutionFunction==nullptr)
								{
									throw string()+"internal error: token resolution function not defined, and it is needed for empty argument";
								}
								argument=tokenResolutionFunction(string(),context,EvaluatorContext(code,t,subexpressionLevel));
							}
							else
							{
								argument=evaluateExpressionPart(code,t,context,0,subexpressionLevel+1);
							}
							arguments.push_back(argument);
							argumentsWithoutCheck++;
							
							if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
							
							parseOperatorDelimiter(code,t,isSeparator,isTerminator,op);
							if(isSeparator)
							{
								if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
							}
							else if(isTerminator) break;
							else throw expectedSeparatorOrTerminatorMessage(op,code.tokens[t],__LINE__);
						}
						
						return op->execute(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
					}
					ValueT evaluateExpressionPart(const CodeT& code,size_t& t,ContextT& context,int level,int subexpressionLevel)
					{
						if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
						
						ValueT lvalue;
						
						Operator*bracketOperator=nullptr;
						if(parseOperator(code,t,bracketOperator,bracketOperators))
						{
							if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
							
							lvalue=executeVariableArgumentOperatorEvaluatingArguments(code,t,context,bracketOperator,vector<ValueT>(),subexpressionLevel);
						}
						else
						{
							Operator*unaryOperator;
							if(parseOperator(code,t,unaryOperator,unaryOperators))
							{
								if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
								
								vector<ValueT> arguments;
								arguments=unaryOperator->executeCheck(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
								
								arguments.push_back(evaluateExpressionPart(code,t,context,getUnaryOperatorLevel(),subexpressionLevel+1));
								
								lvalue=unaryOperator->execute(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
							}
							else
							{
								if(tokenResolutionFunction==nullptr) throw unexpectedTokenMessage(code.tokens[t],__LINE__);
								lvalue=tokenResolutionFunction(code.tokens[t],context,EvaluatorContext(code,t,subexpressionLevel));
								t++;
							}
						}
						
						for(;;)
						{
							if(t>=code.tokens.size()) break;
							if(parseCheckDelimiter(code,t)) break;
							
							{
								Operator*ternaryOperator;
								if(parseOperator(code,t,ternaryOperator,ternaryOperators))
								{
									if(getTernaryOperatorLevel()>level)
									{
										if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
										
										vector<ValueT> arguments=vector<ValueT>{lvalue};
										arguments=ternaryOperator->executeCheck(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
										
										arguments.push_back(evaluateExpressionPart(code,t,context,0,subexpressionLevel+1));
										arguments=ternaryOperator->executeCheck(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
										
										if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
										
										bool isSeparator=false;
										bool isTerminator=false;
										parseOperatorDelimiter(code,t,isSeparator,isTerminator,ternaryOperator);
										if(!isSeparator) throw expectedSeparatorMessage(ternaryOperator,code.tokens[t],__LINE__);
										
										if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
										
										arguments.push_back(evaluateExpressionPart(code,t,context,0,subexpressionLevel+1));
										
										lvalue=ternaryOperator->execute(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
									}
									
									break;
								}
							}
							
							{
								int operatorLevel=getBracketFunctionOperatorLevel();
								if(operatorLevel>level)
								{
									Operator*bracketFunctionOperator;
									if(parseOperator(code,t,bracketFunctionOperator,bracketFunctionOperators))
									{
										if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
										
										vector<ValueT> arguments=vector<ValueT>{lvalue};
										
										arguments=bracketFunctionOperator->executeCheck(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
										
										lvalue=executeVariableArgumentOperatorEvaluatingArguments(code,t,context,bracketFunctionOperator,arguments,subexpressionLevel);
										
										continue;
									}
								}
							}
							
							{
								Operator*binaryOperator;
								size_t tCopy=t;
								if(parseOperator(code,t,binaryOperator,binaryOperators))
								{
									int operatorLevel=binaryOperator->getOperatorLevel();
									if(operatorLevel>level)
									{
										if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
										
										vector<ValueT> arguments=binaryOperator->executeCheck(vector<ValueT>{lvalue},context,EvaluatorContext(code,t,subexpressionLevel));
										
										arguments.push_back(evaluateExpressionPart(code,t,context,operatorLevel,subexpressionLevel+1));
										
										lvalue=binaryOperator->execute(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
										
										continue;
									}
									else
									{
										t=tCopy;
										break;
									}
								}
								else throw unexpectedTokenMessage(code.tokens[t],__LINE__);
							}
						}
						
						return lvalue;
					}
				public:
				
				ValueT evaluate(const string& expressionString,ContextT& context)
				{
					if(changed) updateChanges();
					
					CodeT code(expressionString);
					size_t t=0;
					ValueT result=evaluateExpressionPart(code,t,context,0,0);
					
					if(t<code.tokens.size()) throw unexpectedTokenMessage(code.tokens[t],__LINE__);
					
					return result;
				}
			};
			
			class TemplateExpressionEvaluator
			{
				class Context
				{
					public:
					
					vector<TemplateArgument> variables;
					
					private:
						bool evaluate=true;
						int subexpressionLevelToEvaluateAgain=0;
					public:
					
					Context(){}
					explicit Context(const vector<TemplateArgument>& _variables)
					{
						variables=_variables;
					}
					
					bool updateEvaluationState(int subexpressionLevel)
					{
						if(!evaluate && subexpressionLevel<=subexpressionLevelToEvaluateAgain)
						{
							evaluate=true;
							subexpressionLevelToEvaluateAgain=0;
						}
						return evaluate;
					}
					void shortCircuit(int subexpressionLevel)
					{
						if(evaluate)
						{
							evaluate=false;
							subexpressionLevelToEvaluateAgain=subexpressionLevel;
						}
					}
				};
				
				private:
					using ExpressionEvaluatorType=ExpressionEvaluator<string,Context,TokenizedCode>;
					
					ExpressionEvaluatorType expressionEvaluator;
					
					using Operator=ExpressionEvaluatorType::Operator;
					using EvaluatorContext=ExpressionEvaluatorType::EvaluatorContext;
					
					static int stringToInt(const string& str)
					{
						int intValue=0;
						if(!stringToIntSimple(str,intValue)) throw string()+"could not convert the string '"+str+"' to integer";
						return intValue;
					}
					static string intToString(int intValue)
					{
						return std::to_string(intValue);
					}
					static SafeInteger stringToSafeInteger(const string& str)
					{
						return SafeInteger(stringToInt(str));
					}
					static string safeIntegerToString(const SafeInteger& value)
					{
						return std::to_string(value.getValue());
					}
				public:
				
				TemplateExpressionEvaluator()
				{
					try
					{
						std::function<string(const string&,Context&,const EvaluatorContext&)> tokenResolutionFunction=
							[](const string& token,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								if(token.size()>0 && token[0]>='0' && token[0]<='9')
								{
									return intToString(stringToInt(token));
								}
								else
								{
									int variableIndex=findWithName(context.variables,token);
									if(variableIndex==-1) throw string()+"variable named '"+token+"' not found";
									
									return context.variables[variableIndex].value;
								}
							};
						
						vector<Operator> unaryOperators=vector<Operator>
						{
							Operator("-",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								
								SafeInteger r=-a;
								
								return safeIntegerToString(r);
							}),
							Operator("~",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								
								SafeInteger r=~a;
								
								return safeIntegerToString(r);
							}),
							Operator("!",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								
								SafeInteger r=!a;
								
								return safeIntegerToString(r);
							})
						};
						
						vector<Operator> binaryOperators=vector<Operator>
						{
							Operator("**",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r=a.pow(b);
								
								return safeIntegerToString(r);
							}),
							Operator("*",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r=a*b;
								
								return safeIntegerToString(r);
							}),
							Operator("/",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r=a/b;
								
								return safeIntegerToString(r);
							}),
							Operator("%",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r=a%b;
								
								return safeIntegerToString(r);
							}),
							Operator("+",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r=a+b;
								
								return safeIntegerToString(r);
							}),
							Operator("-",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r=a-b;
								
								return safeIntegerToString(r);
							}),
							Operator("<<",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r=(a<<b);
								
								return safeIntegerToString(r);
							}),
							Operator(">>",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r=(a>>b);
								
								return safeIntegerToString(r);
							}),
							Operator(">>>",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r=a.shiftRightLogical(b);
								
								return safeIntegerToString(r);
							}),
							Operator("<",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r= a<b;
								
								return safeIntegerToString(r);
							}),
							Operator("<=",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r= a<=b;
								
								return safeIntegerToString(r);
							}),
							Operator(">",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r= a>b;
								
								return safeIntegerToString(r);
							}),
							Operator(">=",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r= a>=b;
								
								return safeIntegerToString(r);
							}),
							Operator("==",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r= a==b;
								
								return safeIntegerToString(r);
							}),
							Operator("!=",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r= a!=b;
								
								return safeIntegerToString(r);
							}),
							Operator("&",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r= a&b;
								
								return safeIntegerToString(r);
							}),
							Operator("^",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r= a^b;
								
								return safeIntegerToString(r);
							}),
							Operator("|",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								SafeInteger a=stringToSafeInteger(args[0]);
								SafeInteger b=stringToSafeInteger(args[1]);
								
								SafeInteger r= a|b;
								
								return safeIntegerToString(r);
							}),
							Operator("&&",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								if(!stringToInt(args[0])) return intToString(0);
								return intToString(stringToInt(args[1])!=0);
							},
								[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->vector<string>
								{
									if(args.size()>0)
									{
										if(!stringToInt(args[0])) context.shortCircuit(evaluatorContext.subexpressionLevel);
									}
									return args;
								}
							),
							Operator("||",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								if(stringToInt(args[0])) return intToString(1);
								return intToString(stringToInt(args[1])!=0);
							},
								[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->vector<string>
								{
									if(args.size()>0)
									{
										if(stringToInt(args[0])) context.shortCircuit(evaluatorContext.subexpressionLevel);
									}
									return args;
								}
							)
						};
						vector<vector<string>> binaryOperatorLevels=vector<vector<string>>
						{
							vector<string>{"**"},
							vector<string>{"*","/","%"},
							vector<string>{"+","-"},
							vector<string>{"<<",">>",">>>"},
							vector<string>{"<","<=",">",">="},
							vector<string>{"==","!="},
							vector<string>{"&"},
							vector<string>{"^"},
							vector<string>{"|"},
							vector<string>{"&&"},
							vector<string>{"||"}
						};
						
						vector<Operator> ternaryOperators=vector<Operator>{
							Operator(vector<string>{"?",":"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								return stringToInt(args[0]) ? args[1] : args[2];
							},
								[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->vector<string>
								{
									if(args.size()==1)
									{
										if(!stringToInt(args[0])) context.shortCircuit(evaluatorContext.subexpressionLevel);
									}
									else if(args.size()==2)
									{
										if(stringToInt(args[0])) context.shortCircuit(evaluatorContext.subexpressionLevel);
										else context.updateEvaluationState(evaluatorContext.subexpressionLevel);
									}
									return args;
								}
							)
						};
						
						vector<Operator> bracketOperators=vector<Operator>{
							Operator(vector<string>{"(",")"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								return args[0];
							},1),
							Operator(vector<string>{"isp2(",",",")"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								int a=stringToInt(args[0]);
								int r=a>0 && std::has_single_bit(uint32_t(a));
								
								return intToString(r);
							},1),
							Operator(vector<string>{"log2(",",",")"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								int a=stringToInt(args[0]);
								if(a<=0) throw string()+"logarithm of zero or negative number: "+std::to_string(a);
								int r=int(std::countr_zero(std::bit_floor(uint32_t(a))));
								
								return intToString(r);
							},1),
							Operator(vector<string>{"p2floor(",",",")"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								int a=stringToInt(args[0]);
								if(a<=0) throw string()+"invalid zero or negative number for function: "+std::to_string(a);
								int r=int(std::bit_floor(uint32_t(a)));
								
								return intToString(r);
							},1),
							Operator(vector<string>{"p2ceil(",",",")"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
							{
								if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
								
								int a=stringToInt(args[0]);
								if(a<=0 || a>(1<<30)) throw string()+"invalid out of range number for function: "+std::to_string(a);
								int r=int(std::bit_ceil(uint32_t(a)));
								
								return intToString(r);
							},1)
						};
						
						expressionEvaluator.setTokenResolutionFunction(tokenResolutionFunction);
						expressionEvaluator.setUnaryOperators(unaryOperators);
						expressionEvaluator.setBinaryOperators(binaryOperators,binaryOperatorLevels);
						expressionEvaluator.setTernaryOperators(ternaryOperators);
						expressionEvaluator.setBracketOperators(bracketOperators);
					}
					catch(const string& str)
					{
						throw string()+"Internal error in template expression evaluator initialization: "+str;
					}
				}
				
				string evaluate(const string& expressionString,const vector<TemplateArgument>& templateArguments)
				{
					Context context(templateArguments);
					return expressionEvaluator.evaluate(expressionString,context);
				}
			};
			
			TemplateExpressionEvaluator templateExpressionEvaluator;
			
			string getTemplateArgumentExpressionResult(const string& expressionString,const vector<TemplateArgument>& templateArguments,int lineIndex)
			{
				try
				{
					return templateExpressionEvaluator.evaluate(expressionString,templateArguments);
				}
				catch(const string& str)
				{
					throw errorString(string()+"Error in template expression evaluation: "+str+"\nThe expression is: "+expressionString,lineIndex);
				}
			}
			
			class InputExpression
			{
				public:
				
				class ValueMetadata
				{
					public:
					
					int size=-1;
					
					ValueMetadata(){}
					explicit ValueMetadata(int _size)
					{
						size=_size;
					}
				};
				
				enum class Type{none,integerConstant,variable,operation};
				
				Type type=Type::none;
				
				
				int integerConstantValue=0;
				
				int variableIndex=-1;
				
				int operationIndex=-1;
				vector<InputExpression> operationArguments;
				vector<int> operationIntegerArguments;
				
				
				ValueMetadata resultMetadata;
				
				InputExpression(){}
				explicit InputExpression(int _integerConstantValue)
				{
					type=Type::integerConstant;
					integerConstantValue=_integerConstantValue;
				}
				InputExpression(const Component::Variable& variable,int _variableIndex)
				{
					type=Type::variable;
					variableIndex=_variableIndex;
					resultMetadata=ValueMetadata(variable.sizeInBits);
				}
				InputExpression(int _operationIndex,
					const vector<InputExpression>& _operationArguments,const vector<int>& _operationIntegerArguments,
					const ValueMetadata& _resultMetadata)
				{
					type=Type::operation;
					operationIndex=_operationIndex;
					operationArguments=_operationArguments;
					operationIntegerArguments=_operationIntegerArguments;
					resultMetadata=_resultMetadata;
				}
				bool isIntegerConstant() const
				{
					return type==Type::integerConstant;
				}
				bool isBitArray() const
				{
					return type==Type::variable || type==Type::operation;
				}
			};
			
			class InputExpressionEvaluator
			{
				public:
				
				class BitOrigin
				{
					public:
					
					enum class Type{constant,variable};
					
					Type type=Type::constant;
					
					int constantValue=0;
					
					int variableIndex=-1;
					int variableBitIndex=-1;
					
					BitOrigin(){}
					explicit BitOrigin(int _constantValue)
					{
						constantValue=_constantValue;
					}
					BitOrigin(int _variableIndex,int _variableBitIndex)
					{
						type=Type::variable;
						variableIndex=_variableIndex;
						variableBitIndex=_variableBitIndex;
					}
				};
				
				class BitOriginVector
				{
					public:
					
					vector<BitOrigin> content;
					
					BitOriginVector(){}
					explicit BitOriginVector(const vector<BitOrigin>& _content)
					{
						content=_content;
					}
					BitOriginVector(int size,int value)
					{
						content=vector<BitOrigin>(size);
						for(int i=0;i<content.size();i++)
						{
							if(i<32) content[i].constantValue=(uint32_t(value)>>i)&1;
							else content[i].constantValue=(uint32_t(value)>>31)&1;
						}
					}
					explicit BitOriginVector(const InputExpression& variableInputExpression)
					{
						if(variableInputExpression.type!=InputExpression::Type::variable)
						{
							throw string()+"internal error: trying to get a BitOriginVector from an InputExpression that is not of variable type";
						}
						
						content=vector<BitOrigin>(variableInputExpression.resultMetadata.size);
						for(int i=0;i<content.size();i++)
						{
							content[i]=BitOrigin(variableInputExpression.variableIndex,i);
						}
					}
					
					BitOriginVector subVector(const vector<int>& indexingVector) const
					{
						if(indexingVector.size()!=2) throw string()+"internal error: indexing vector sizes do not match with subvector function";
						
						if(indexingVector[0]<0 || indexingVector[0]>=content.size() || indexingVector[1]<0 || indexingVector[1]>content.size())
						{
							throw string()+"internal error: indexing vector values do not match with content size in subvector function";
						}
						
						return BitOriginVector(vector<BitOrigin>(content.begin()+indexingVector[0],content.begin()+indexingVector[1]));
					}
					
					static BitOriginVector concatenate(const BitOriginVector& a,const BitOriginVector& b)
					{
						BitOriginVector r=a;
						r.content.insert(r.content.end(),b.content.begin(),b.content.end());
						return r;
					}
					static BitOriginVector reverse(const BitOriginVector& a)
					{
						BitOriginVector r=a;
						std::reverse(r.content.begin(),r.content.end());
						return r;
					}
					static BitOriginVector repeat(const BitOriginVector& a,int b)
					{
						if(b==0) throw string()+"internal error: BitOriginVector::repeat 0 times";
						
						if(b<0)
						{
							if(b==int(uint32_t(1)<<31)) throw string()+"internal error: BitOriginVector::repeat <minimum negative> times";
							
							return repeat(reverse(a),-b);
						}
						else
						{
							BitOriginVector r=a;
							r.content.reserve(a.content.size()*b);
							for(int i=1;i<b;i++)
							{
								r.content.insert(r.content.end(),a.content.begin(),a.content.end());
							}
							return r;
						}
					}
					static BitOriginVector transpose(const BitOriginVector& a,int b)
					{
						int aSize=a.content.size();
						
						if(b<=0 || b>aSize || aSize%b!=0)
						{
							throw string()+"internal error: BitOriginVector::transpose with: size="
								+std::to_string(a.content.size())+", width="+std::to_string(b);
						}
						
						BitOriginVector r;
						r.content=vector<BitOrigin>(a.content.size());
						for(int i=0;i<(aSize/b);i++)
						{
							for(int j=0;j<b;j++)
							{
								r.content[j*(aSize/b)+i]=a.content[i*b+j];
							}
						}
						return r;
					}
				};
				
				private:
					class ParserContext
					{
						public:
						
						NamedVector<Component::Variable>*variablesPtr=nullptr;
						
						ParserContext(){}
						explicit ParserContext(NamedVector<Component::Variable>& _variables)
						{
							variablesPtr=&_variables;
						}
					};
					
					using ParserExpressionEvaluatorType=ExpressionEvaluator<InputExpression,ParserContext,TokenizedCode>;
					
					using ParserOperator=ParserExpressionEvaluatorType::Operator;
					using ParserEvaluatorContext=ParserExpressionEvaluatorType::EvaluatorContext;
					
					using EvaluatorFunction=std::function<BitOriginVector(const vector<BitOriginVector>&,const vector<int>&)>;
					
					class Operation
					{
						public:
						
						enum class Type{none,unaryOperator,binaryOperator,ternaryOperator,bracketOperator,bracketFunctionOperator};
						
						Type type=Type::none;
						ParserOperator parserOperator;
						EvaluatorFunction evaluatorFunction;
						
						Operation(){}
						Operation(const Type& _type,const ParserOperator& _parserOperator,const EvaluatorFunction& _evaluatorFunction)
						{
							type=_type;
							parserOperator=_parserOperator;
							evaluatorFunction=_evaluatorFunction;
						}
					};
					
					class Parser
					{
						private:
							using ExpressionEvaluatorType=ParserExpressionEvaluatorType;
							
							ExpressionEvaluatorType expressionEvaluator;
							
							using Operator=ExpressionEvaluatorType::Operator;
							using EvaluatorContext=ExpressionEvaluatorType::EvaluatorContext;
							
							static int stringToInt(const string& str)
							{
								int intValue=0;
								if(!stringToIntSimple(str,intValue)) throw string()+"could not convert the string '"+str+"' to integer";
								return intValue;
							}
						public:
						
						Parser(){}
						Parser(vector<Operation>& operations)
						{
							std::function<InputExpression(const string&,ParserContext&,const EvaluatorContext&)> tokenResolutionFunction=
								[](const string& token,ParserContext& context,const EvaluatorContext& evaluatorContext)->InputExpression
								{
									if(token.size()>0 && token[0]>='0' && token[0]<='9')
									{
										return InputExpression(stringToInt(token));
									}
									else
									{
										if(context.variablesPtr==nullptr)
										{
											throw string()+"Internal error: the pointer to the variables is null";
										}
										
										int variableIndex=findWithName(*context.variablesPtr,token);
										if(variableIndex==-1) throw string()+"variable named '"+token+"' not found";
										
										const Component::Variable& variable=(*context.variablesPtr)[variableIndex];
										if(variable.type==Component::Variable::Type::output)
										{
											throw string()+"reading from output variable: '"+token+"'";
										}
										
										return InputExpression(variable,variableIndex);
									}
								};
							vector<Operator> unaryOperators=vector<Operator>{
								Operator("-",[this](const vector<InputExpression>& args)->InputExpression
								{
									if(!args[0].isIntegerConstant()) throw string()+"expected integer for sign change";
									
									int a=args[0].integerConstantValue;
									
									int r=(-SafeInteger(a)).getValue();
									
									return InputExpression(r);
								})
							};
							vector<Operator> binaryOperators=vector<Operator>{};
							vector<vector<string>> binaryOperatorLevels=vector<vector<string>>
							{
								vector<string>{"**"},
								vector<string>{"*","/","%"},
								vector<string>{"+","-"},
								vector<string>{"<<",">>",">>>"},
								vector<string>{"<","<=",">",">="},
								vector<string>{"==","!="},
								vector<string>{"&"},
								vector<string>{"^"},
								vector<string>{"|"},
								vector<string>{"&&"},
								vector<string>{"||"}
							};
							vector<Operator> ternaryOperators=vector<Operator>{};
							vector<Operator> bracketFunctionOperators=vector<Operator>{};
							vector<Operator> bracketOperators=vector<Operator>{
								Operator(vector<string>{"(",")"},[this](const vector<InputExpression>& args)->InputExpression
								{
									return args[0];
								},1)
							};
							
							for(int i=0;i<operations.size();i++)
							{
								Operation& op=operations[i];
								if(op.type==Operation::Type::unaryOperator) unaryOperators.push_back(op.parserOperator);
								else if(op.type==Operation::Type::binaryOperator) binaryOperators.push_back(op.parserOperator);
								else if(op.type==Operation::Type::ternaryOperator) ternaryOperators.push_back(op.parserOperator);
								else if(op.type==Operation::Type::bracketFunctionOperator) bracketFunctionOperators.push_back(op.parserOperator);
								else if(op.type==Operation::Type::bracketOperator) bracketOperators.push_back(op.parserOperator);
							}
							
							expressionEvaluator.setTokenResolutionFunction(tokenResolutionFunction);
							expressionEvaluator.setUnaryOperators(unaryOperators);
							expressionEvaluator.setBinaryOperators(binaryOperators,binaryOperatorLevels);
							expressionEvaluator.setTernaryOperators(ternaryOperators);
							expressionEvaluator.setBracketFunctionOperators(bracketFunctionOperators);
							expressionEvaluator.setBracketOperators(bracketOperators);
						}
						
						InputExpression evaluate(const string& expressionString,NamedVector<Component::Variable>& variables)
						{
							ParserContext context(variables);
							return expressionEvaluator.evaluate(expressionString,context);
						}
					};
					
					vector<Operation> operations;
					Parser parser;
					
					void addOperation(int& operationIndex,const Operation& op)
					{
						operations.emplace_back(op);
						operationIndex++;
					}
				public:
				
				InputExpressionEvaluator()
				{
					try
					{
						int operationIndex=0;
						
						addOperation(operationIndex,
							Operation(
								Operation::Type::bracketFunctionOperator,
								ParserOperator(vector<string>{"[",":","]"},[this,operationIndex](const vector<InputExpression>& args)->InputExpression
								{
									if(args.size()<2 || args.size()>3) throw string()+"invalid parameter count for bit range";
									
									if(!args[0].isBitArray())
									{
										throw string()+"expected bit array for bit range";
									}
									
									if(!args[1].isIntegerConstant()) throw string()+"expected integer for bit range";
									if(args.size()>2 && !args[2].isIntegerConstant()) throw string()+"expected integer for bit range";
									
									int inputSize=args[0].resultMetadata.size;
									if(inputSize==-1) throw string()+"internal error: input size not defined";
									
									int start=args[1].integerConstantValue;
									int end_=start+1;
									if(args.size()>2) end_=args[2].integerConstantValue;
									
									if(start<0 || start>=inputSize)
									{
										throw string()+"bit range start out of range: "+std::to_string(start)+" (size="+std::to_string(inputSize)+")";
									}
									if(end_<=0 || end_>inputSize)
									{
										throw string()+"bit range end out of range: "+std::to_string(end_)+" (size="+std::to_string(inputSize)+")";
									}
									if(start>=end_)
									{
										throw string()+"invalid bit range: start and end are in an invalid order: "
											+"["+std::to_string(start)+":"+std::to_string(end_)+"] (size="+std::to_string(inputSize)+")";
									}
									
									vector<int> indexingVector=vector<int>{start,end_};
									
									return InputExpression(operationIndex,vector<InputExpression>{args[0]},indexingVector,InputExpression::ValueMetadata(end_-start));
								},-1,false),
								[this](const vector<BitOriginVector>& args,const vector<int>& intArgs)->BitOriginVector
								{
									return args[0].subVector(intArgs);
								}
							));
						addOperation(operationIndex,
							Operation(
								Operation::Type::binaryOperator,
								ParserOperator("*",[this,operationIndex](const vector<InputExpression>& args)->InputExpression
								{
									if(!args[0].isBitArray())
									{
										throw string()+"expected bit array for repetition";
									}
									if(!args[1].isIntegerConstant())
									{
										throw string()+"expected integer constant for repetition";
									}
									
									int a=args[0].resultMetadata.size;
									int b=args[1].integerConstantValue;
									
									if(b==0)
									{
										throw string()+"invalid repetition factor: "+std::to_string(b);
									}
									
									SafeInteger aSize(a);
									SafeInteger bRepetitions(b);
									if((bRepetitions<SafeInteger(0)).getValue())
									{
										bRepetitions=-bRepetitions;
									}
									int outputSize=(aSize*bRepetitions).getValue();
									
									return InputExpression(operationIndex,vector<InputExpression>{args[0]},vector<int>{b},InputExpression::ValueMetadata(outputSize));
								}),
								[this](const vector<BitOriginVector>& args,const vector<int>& intArgs)->BitOriginVector
								{
									return BitOriginVector::repeat(args[0],intArgs[0]);
								}
							));
						addOperation(operationIndex,
							Operation(
								Operation::Type::binaryOperator,
								ParserOperator("%",[this,operationIndex](const vector<InputExpression>& args)->InputExpression
								{
									if(!args[0].isBitArray())
									{
										throw string()+"expected bit array for transposition";
									}
									if(!args[1].isIntegerConstant())
									{
										throw string()+"expected integer constant for transposition";
									}
									
									int a=args[0].resultMetadata.size;
									int b=args[1].integerConstantValue;
									
									if(b<0)
									{
										throw string()+"invalid transposition size: arraysize="
											+std::to_string(a)+" matrixwidth="+std::to_string(b)
											+" ("+std::to_string(b)+" is negative"+")";
									}
									if(b==0 || b>a || a%b!=0)
									{
										throw string()+"invalid transposition size: arraysize="
											+std::to_string(a)+" matrixwidth="+std::to_string(b)
											+" ("+std::to_string(a)+" is not divisible by "+std::to_string(b)+")";
									}
									
									int outputSize=a;
									
									return InputExpression(operationIndex,vector<InputExpression>{args[0]},vector<int>{b},InputExpression::ValueMetadata(outputSize));
								}),
								[this](const vector<BitOriginVector>& args,const vector<int>& intArgs)->BitOriginVector
								{
									return BitOriginVector::transpose(args[0],intArgs[0]);
								}
							));
						addOperation(operationIndex,
							Operation(
								Operation::Type::binaryOperator,
								ParserOperator("+",[this,operationIndex](const vector<InputExpression>& args)->InputExpression
								{
									if(!args[0].isBitArray() || !args[1].isBitArray())
									{
										throw string()+"expected bit arrays for concatenation";
									}
									
									int a=args[0].resultMetadata.size;
									int b=args[1].resultMetadata.size;
									
									int outputSize=(SafeInteger(a)+SafeInteger(b)).getValue();
									
									return InputExpression(operationIndex,args,vector<int>(),InputExpression::ValueMetadata(outputSize));
								}),
								[this](const vector<BitOriginVector>& args,const vector<int>& intArgs)->BitOriginVector
								{
									return BitOriginVector::concatenate(args[0],args[1]);
								}
							));
						
						parser=Parser(operations);
					}
					catch(const string& str)
					{
						throw string()+"Internal error in input expression evaluator initialization: "+str;
					}
				}
				
				InputExpression parse(const string& expressionString,NamedVector<Component::Variable>& variables,int expectedSize)
				{
					InputExpression expression=parser.evaluate(expressionString,variables);
					
					if(expression.type==InputExpression::Type::none)
					{
						throw string()+"internal error: input expression of type none after parsing";
					}
					
					if(expression.type==InputExpression::Type::integerConstant)
					{
						expression.resultMetadata=InputExpression::ValueMetadata(expectedSize);
					}
					else if(expression.resultMetadata.size!=expectedSize)
					{
						throw string()+"input expression size ("+std::to_string(expression.resultMetadata.size)
							+") does not match the expected size for the component parameter ("
							+std::to_string(expectedSize)+")";
					}
					return expression;
				}
				BitOriginVector evaluate(const InputExpression& expression)
				{
					if(expression.type==InputExpression::Type::integerConstant)
					{
						return BitOriginVector(expression.resultMetadata.size,expression.integerConstantValue);
					}
					if(expression.type==InputExpression::Type::variable)
					{
						return BitOriginVector(expression);
					}
					if(expression.type==InputExpression::Type::operation)
					{
						if(expression.operationIndex<0 || expression.operationIndex>=operations.size())
						{
							throw string()+"internal error: input expression operation index out of range";
						}
						
						vector<BitOriginVector> args;
						for(int i=0;i<expression.operationArguments.size();i++)
						{
							args.emplace_back(evaluate(expression.operationArguments[i]));
						}
						
						return operations[expression.operationIndex].evaluatorFunction(args,expression.operationIntegerArguments);
					}
					else
					{
						throw string()+"internal error: input expression of invalid type";
					}
				}
			};
			
			InputExpressionEvaluator inputExpressionEvaluator;
			
			InputExpression parseInputExpression(const string& expressionString,NamedVector<Component::Variable>& variables,int expectedSize,int lineIndex)
			{
				try
				{
					return inputExpressionEvaluator.parse(expressionString,variables,expectedSize);
				}
				catch(const string& str)
				{
					throw errorString(string()+"Error in input expression parsing: "+str+"\nThe expression is: "+expressionString,lineIndex);
				}
			}
			InputExpressionEvaluator::BitOriginVector evaluateInputExpression(const InputExpression& expression,int lineIndex)
			{
				try
				{
					return inputExpressionEvaluator.evaluate(expression);
				}
				catch(const string& str)
				{
					throw errorString(string()+"Error in input expression evaluation: "+str,lineIndex);
				}
			}
			
			string invalidSplittedTemplateLineMessage(int errorCode)
			{
				return "Template delimiters ('<' and '>') do not match";
			}
			string invalidIdentifierMessage(const string& identifier,int errorCode)
			{
				return string()+"Invalid name '"+identifier+"'";
			}
			string expectedContinuationOfLineMessage(int errorCode)
			{
				return "Expected continuation of line";
			}
			
			void processTemplate(size_t& originalLineIndex)
			{
				size_t startOriginalLineIndex=originalLineIndex;
				vector<string> templateLines;
				
				for(;originalLineIndex<originalLines.size();originalLineIndex++)
				{
					string line=originalLines[originalLineIndex];
					
					if(originalLineIndex>startOriginalLineIndex)
					{
						if(line.size()>0)
						{
							if(line.back()==':')
							{
								break;
							}
						}
					}
					
					templateLines.emplace_back(line);
				}
				
				vector<TemplateParameter> templateParameters;
				int numberOfCombinations=1;
				{
					size_t lineIndex=startOriginalLineIndex;
					
					string startLine=templateLines[0];
					vector<string> startLineSplitted=splitLineTemplates(startLine);
					if(!checkSplittedTemplateLineValid(startLineSplitted)) throw errorString(invalidSplittedTemplateLineMessage(__LINE__),lineIndex);
					for(int i=0;i<startLineSplitted.size();i++)
					{
						string str=startLineSplitted[i];
						if(str[0]=='<')
						{
							str=str.substr(1,str.size()-2);
							
							bool putResult=true;
							
							if(str.size()>0 && str[0]=='/')
							{
								putResult=false;
								str=str.substr(1,str.size()-1);
							}
							
							size_t equalSign=str.find_first_of("=");
							if(equalSign==string::npos) throw errorString("Expected '='",lineIndex);
							
							string name=str.substr(0,equalSign);
							
							if(!isValidIdentifier(name)) throw errorString(invalidIdentifierMessage(name,__LINE__),lineIndex);
							
							vector<string> valuesStr=splitStringAtCommas(str.substr(equalSign+1,str.size()-(equalSign+1)));
							
							vector<string> values;
							
							if(valuesStr.size()==0) throw errorString("The list of values is empty",lineIndex);
							for(int j=0;j<valuesStr.size();j++)
							{
								string value=trimString(valuesStr[j]);
								if(value.size()==0) throw errorString("Empty string values are not allowed",lineIndex);
								for(int k=0;k<value.size();k++)
								{
									if(value[k]==' ') throw errorString("Space in a value",lineIndex);
								}
								
								string dotsString="...";
								
								size_t dots=value.find(dotsString);
								if(dots!=string::npos)
								{
									string startStr=value.substr(0,dots);
									string endStr=value.substr(dots+dotsString.size(),value.size()-(dots+dotsString.size()));
									
									int start=0;
									int end=0;
									if(!stringToIntSimple(startStr,start)) throw errorString("Not an integer for range start",lineIndex);
									if(!stringToIntSimple(endStr,end)) throw errorString("Not an integer for range end",lineIndex);
									
									if(start>end) throw errorString("Range end must be greater or equal to range start",lineIndex);
									
									for(int number=start;number<=end;number++)
									{
										values.push_back(std::to_string(number));
									}
								}
								else
								{
									values.push_back(value);
								}
							}
							
							if(findWithName(templateParameters,name)!=-1) throw errorString(string()+"Template argument '"+name+"' already defined",lineIndex);
							
							if(!putResult && values.size()>1) throw errorString(string()+"Hidden template argument '"+name+"' with multiple values",lineIndex);
							
							templateParameters.emplace_back(name,values);
						}
					}
					
					constexpr int maximumNumberOfCombinations=65536;
					
					numberOfCombinations=1;
					for(size_t i=0;i<templateParameters.size();i++)
					{
						size_t count=templateParameters[i].possibleValues.size();
						
						bool tooManyCombinations=false;
						
						if(count>maximumNumberOfCombinations) tooManyCombinations=true;
						numberOfCombinations*=count;
						if(numberOfCombinations>maximumNumberOfCombinations) tooManyCombinations=true;
						
						if(tooManyCombinations)
						{
							throw errorString(string()+"Too many template argument value combinations: >"+std::to_string(maximumNumberOfCombinations),lineIndex);
						}
					}
				}
				
				vector<TemplateArgument> templateArguments(templateParameters.size());
				for(int i=0;i<templateParameters.size();i++)
				{
					templateArguments[i].name=templateParameters[i].name;
				}
				for(int combination=0;combination<numberOfCombinations;combination++)
				{
					{
						int base=1;
						for(int i=0;i<templateParameters.size();i++)
						{
							int index=(combination/base)%templateParameters[i].possibleValues.size();
							
							base*=templateParameters[i].possibleValues.size();
							
							templateArguments[i].value=templateParameters[i].possibleValues[index];
						}
					}
					
					bool ignoreThisCombination=false;
					vector<string> finalLines;
					
					for(size_t templateLineIndex=0;templateLineIndex<templateLines.size();templateLineIndex++)
					{
						string line=templateLines[templateLineIndex];
						
						size_t lineIndex=startOriginalLineIndex+templateLineIndex;
						
						string finalLine;
						
						bool ignoreThisLine=false;
						
						if(line.size()>0)
						{
							vector<string> lineSplitted=splitLineTemplates(line);
							if(!checkSplittedTemplateLineValid(lineSplitted)) throw errorString(invalidSplittedTemplateLineMessage(__LINE__),lineIndex);
							
							for(size_t i=0;i<lineSplitted.size();i++)
							{
								string str=lineSplitted[i];
								if(str[0]=='<')
								{
									str=str.substr(1,str.size()-2);
									
									if(str.size()==0) throw errorString("Empty template expression",lineIndex);
									
									bool putResult=false;
									bool combinationAssertMode=false;
									bool lineAssertMode=false;
									if(str[0]=='?')
									{
										combinationAssertMode=true;
										str=str.substr(1,str.size()-1);
									}
									else if(str[0]==':')
									{
										lineAssertMode=true;
										str=str.substr(1,str.size()-1);
									}
									else if(str[0]=='/')
									{
										if(templateLineIndex!=0) throw errorString("Template expression that does nothing",lineIndex);
										str=str.substr(1,str.size()-1);
									}
									else
									{
										putResult=true;
									}
									
									if(templateLineIndex==0)
									{
										if(putResult)
										{
											size_t equalSign=str.find_first_of("=");
											if(equalSign==string::npos) throw errorString(__LINE__,lineIndex);
											
											string name=str.substr(0,equalSign);
											
											int parameterIndex=findWithName(templateParameters,name);
											if(parameterIndex==-1) throw errorString(__LINE__,lineIndex);
											
											finalLine+=templateArguments[parameterIndex].value;
										}
									}
									else
									{
										string expressionResult=getTemplateArgumentExpressionResult(str,templateArguments,lineIndex);
										
										if(combinationAssertMode)
										{
											if(expressionResult=="0")
											{
												ignoreThisCombination=true;
												break;
											}
										}
										else if(lineAssertMode)
										{
											if(expressionResult=="0")
											{
												ignoreThisLine=true;
												break;
											}
										}
										
										if(putResult)
										{
											finalLine+=expressionResult;
										}
									}
								}
								else
								{
									finalLine+=str;
								}
								
								if(ignoreThisCombination || ignoreThisLine) break;
							}
						}
						
						if(ignoreThisCombination) break;
						if(ignoreThisLine) continue;
						
						finalLines.push_back(finalLine);
					}
					
					if(!ignoreThisCombination)
					{
						for(size_t templateLineIndex=0;templateLineIndex<finalLines.size();templateLineIndex++)
						{
							string line=finalLines[templateLineIndex];
							
							size_t lineIndex=startOriginalLineIndex+templateLineIndex;
							
							addTemplateProcessedLine(line,lineIndex);
						}
					}
				}
			}
			void processTemplates()
			{
				for(size_t originalLineIndex=0;originalLineIndex<originalLines.size();)
				{
					string line=originalLines[originalLineIndex];
					
					if(line.size()>0)
					{
						if(line.back()==':')
						{
							processTemplate(originalLineIndex);
							continue;
						}
					}
					
					addTemplateProcessedLine(line,originalLineIndex);
					originalLineIndex++;
				}
				
				originalLinesProcessed=true;
			}
			
			bool isVariableDeclarationLine(const string& line)
			{
				size_t colon=line.find_first_of(":");
				if(colon==string::npos) return false;
				
				string name=line.substr(0,colon);
				if(!isValidIdentifier(name)) return false;
				
				return true;
			}
			void processVariableDeclarationLine(const string& line,int lineIndex)
			{
				size_t colon=line.find_first_of(":");
				if(colon==string::npos) throw errorString(__LINE__,lineIndex);
				
				string name=line.substr(0,colon);
				if(!isValidIdentifier(name)) throw errorString(invalidIdentifierMessage(name,__LINE__),lineIndex);
				
				size_t position=colon+1;
				if(position>=line.size()) throw errorString(expectedContinuationOfLineMessage(__LINE__),lineIndex);
				vector<string> splitted=splitStringAtSpaces(line.substr(position,line.size()-position));
				
				if(splitted.size()==0) throw errorString("Expected variable type",lineIndex);
				if(splitted.size()>2) throw errorString("Too many parameters for variable",lineIndex);
				string typeString=splitted[0];
				int sizeInBits=1;
				if(splitted.size()>1)
				{
					string str=splitted[1];
					try
					{
						sizeInBits=std::stoi(str);
						if(sizeInBits<=0 || sizeInBits>maximumVariableSizeInBits) throw 1;
						if(std::to_string(sizeInBits)!=str) throw 1;
					}
					catch(...)
					{
						throw errorString(string()+"Invalid variable size: "+str,lineIndex);
					}
				}
				
				if(findWithName(components.back().variables,name)!=-1) throw errorString("Variable name already in use",lineIndex);
				
				Component::Variable::Type type=Component::Variable::Type::input;
				if(typeString=="in"){}
				else if(typeString=="out") type=Component::Variable::Type::output;
				else if(typeString=="reg") type=Component::Variable::Type::reg;
				else throw errorString("Invalid variable type",lineIndex);
				
				components.back().variables.emplace_back(type,name,sizeInBits,lineIndex);
				
				if(type==Component::Variable::Type::input) components.back().inputs.emplace_back(name,sizeInBits);
				else if(type==Component::Variable::Type::output) components.back().outputs.emplace_back(name,sizeInBits);
			}
			
			void readComponents()
			{
				for(size_t lineIndex=0;lineIndex<lines.size();lineIndex++)
				{
					string line=lines[lineIndex];
					
					if(line.size()==0) continue;
					
					if(line.back()==':')
					{
						string name=line.substr(0,line.size()-1);
						if(!isValidIdentifier(name)) throw errorString(invalidIdentifierMessage(name,__LINE__),lineIndex);
						if(findWithName(components,name)!=-1) throw errorString(string()+"Component '"+name+"' already defined",lineIndex);
						
						
						if(components.size()>0) components.back().lastLine=lineIndex-1;
						
						components.emplace_back(name);
						
						components.back().firstLine=lineIndex;
						components.back().lastLine=lines.size()-1;
					}
					else
					{
						if(components.size()==0) throw errorString("Expected component header",lineIndex);
						
						if(isVariableDeclarationLine(line))
						{
							processVariableDeclarationLine(line,lineIndex);
						}
					}
				}
			}
			
			int getSetSizeInBits(const string& setString)
			{
				string name="set";
				if(stringStartsWith(setString,name))
				{
					if(setString.size()==name.size()) return 1;
					
					int value=0;
					if(stringToIntSimple(setString.substr(name.size(),setString.size()-name.size()),value))
					{
						if(value>0 && value<=maximumVariableSizeInBits) return value;
					}
				}
				return -1;
			}
			bool getConcatSizeInBits(const string& concatString,int& sizeA,int& sizeB)
			{
				string name="concat";
				if(stringStartsWith(concatString,name))
				{
					if(concatString.size()==name.size()) return false;
					
					string sizesStr=concatString.substr(name.size(),concatString.size()-name.size());
					
					size_t underscore=sizesStr.find_first_of("_");
					if(underscore==string::npos) return false;
					
					if(!stringToIntSimple(sizesStr.substr(0,underscore),sizeA)) return false;
					if(sizeA<=0 || sizeA>maximumVariableSizeInBits) return false;
					
					if(!stringToIntSimple(sizesStr.substr(underscore+1,sizesStr.size()-(underscore+1)),sizeB)) return false;
					if(sizeB<=0 || sizeB>maximumVariableSizeInBits) return false;
					
					return true;
				}
				return false;
			}
			
			class AssignmentLineData
			{
				public:
				
				int lineIndex=-1;
				vector<string> outputs;
				string componentName;
				vector<string> inputs;
				
				Component::Element::Type type;
				int componentIndex=-1;
				
				int numberOfInstancesOfComponent=1;
				
				vector<int> outputSizes;
				vector<int> inputSizes;
				
				vector<int> outputVariableIndexes;
				
				AssignmentLineData(){}
				AssignmentLineData(int _lineIndex,const vector<string>& _outputs,const string& _componentName,const vector<string>& _inputs,
					const Component::Element::Type& _type,int _componentIndex,int _numberOfInstancesOfComponent,
					const vector<int>& _outputSizes,const vector<int>& _inputSizes,
					const vector<int>& _outputVariableIndexes)
				{
					lineIndex=_lineIndex;
					outputs=_outputs;
					componentName=_componentName;
					inputs=_inputs;
					type=_type;
					componentIndex=_componentIndex;
					numberOfInstancesOfComponent=_numberOfInstancesOfComponent;
					outputSizes=_outputSizes;
					inputSizes=_inputSizes;
					outputVariableIndexes=_outputVariableIndexes;
				}
			};
			
			void processComponents()
			{
				for(int thisComponentIndex=0;thisComponentIndex<components.size();thisComponentIndex++)
				{
					Component& component=components[thisComponentIndex];
					
					vector<AssignmentLineData> assignmentLines;
					
					int nextOutputToDiscardIndex=0;
					
					for(int lineIndex=component.firstLine;lineIndex<=component.lastLine;lineIndex++)
					{
						string line=lines[lineIndex];
						
						if(line.size()==0) continue;
						
						if(!isVariableDeclarationLine(line))
						{
							size_t equalSign=line.find_first_of("=");
							if(equalSign==string::npos) throw errorString("Expected '='",lineIndex);
							
							vector<string> outputs=splitStringAtSpaces(line.substr(0,equalSign));
							if(outputs.size()==0) throw errorString("Expected output variables",lineIndex);
							
							size_t position=equalSign+1;
							if(position>=line.size()) throw errorString(expectedContinuationOfLineMessage(__LINE__),lineIndex);
							vector<string> inputs=splitStringAtSpaces(line.substr(position,line.size()-position));
							
							if(inputs.size()<=1) throw errorString("Expected input variables",lineIndex);
							
							string componentString=inputs[0];
							inputs.erase(inputs.begin());
							
							string componentName;
							int numberOfInstancesOfComponent=1;
							{
								size_t asterisk=componentString.find_first_of("*");
								if(asterisk==string::npos) componentName=componentString;
								else
								{
									componentName=componentString.substr(0,asterisk);
									string numberString=componentString.substr(asterisk+1,componentString.size()-(asterisk+1));
									if(numberString.size()==0) throw errorString("Expected component instance count",lineIndex);
									if(!stringToIntSimple(numberString,numberOfInstancesOfComponent) || numberOfInstancesOfComponent<=0)
									{
										throw errorString("Invalid component instance count",lineIndex);
									}
								}
							}
							
							if(!isValidIdentifier(componentName)) throw errorString("Invalid component name '"+componentName+"'",lineIndex);
							
							
							Component::Element::Type elementType=Component::Element::Type::nand;
							
							vector<int> outputSizes;
							vector<int> inputSizes;
							
							int componentIndex=-1;
							
							if(componentName=="nand")
							{
								elementType=Component::Element::Type::nand;
								outputSizes=vector<int>{1};
								inputSizes=vector<int>{1,1};
							}
							else
							{
								int set_sizeInBits=getSetSizeInBits(componentName);
								if(set_sizeInBits!=-1)
								{
									elementType=Component::Element::Type::set;
									outputSizes=vector<int>{set_sizeInBits};
									inputSizes=vector<int>{set_sizeInBits};
								}
								else
								{
									int concat_sizeA=-1;
									int concat_sizeB=-1;
									if(getConcatSizeInBits(componentName,concat_sizeA,concat_sizeB))
									{
										elementType=Component::Element::Type::concat;
										outputSizes=vector<int>{concat_sizeA+concat_sizeB};
										inputSizes=vector<int>{concat_sizeA,concat_sizeB};
									}
									else
									{
										elementType=Component::Element::Type::component;
										
										componentIndex=findWithName(components,componentName);
										if(componentIndex==-1) throw errorString("Component not found",lineIndex);
										if(componentIndex==thisComponentIndex) throw errorString("Component using itself",lineIndex);
										
										for(int i=0;i<components[componentIndex].inputs.size();i++)
										{
											inputSizes.push_back(components[componentIndex].inputs[i].sizeInBits);
										}
										
										for(int i=0;i<components[componentIndex].outputs.size();i++)
										{
											outputSizes.push_back(components[componentIndex].outputs[i].sizeInBits);
										}
									}
								}
							}
							
							if(inputs.size()!=inputSizes.size()) throw errorString("Input count does not match",lineIndex);
							if(outputs.size()!=outputSizes.size()) throw errorString("Output count does not match",lineIndex);
							
							try
							{
								for(int i=0;i<inputSizes.size();i++)
								{
									inputSizes[i]=(SafeInteger(inputSizes[i])*SafeInteger(numberOfInstancesOfComponent)).getValue();
								}
							}
							catch(const string& str)
							{
								throw errorString("Component input size out of range: "+str,lineIndex);
							}
							
							try
							{
								for(int i=0;i<outputSizes.size();i++)
								{
									outputSizes[i]=(SafeInteger(outputSizes[i])*SafeInteger(numberOfInstancesOfComponent)).getValue();
								}
							}
							catch(const string& str)
							{
								throw errorString("Component output size out of range: "+str,lineIndex);
							}
							
							vector<int> outputVariableIndexes;
							
							for(int i=0;i<outputs.size();i++)
							{
								string name=outputs[i];
								if(!isValidIdentifier(name)) throw errorString(invalidIdentifierMessage(name,__LINE__),lineIndex);
								
								if(name=="_")
								{
									name+=std::to_string(nextOutputToDiscardIndex);
									nextOutputToDiscardIndex++;
								}
								else if(name.size()>=2 && name[0]=='_' && name[1]>='0' && name[1]<='9')
								{
									throw errorString(invalidIdentifierMessage(name,__LINE__),lineIndex);
								}
								
								int variableIndex=findWithName(component.variables,name);
								if(variableIndex!=-1)
								{
									Component::Variable::Type variableType=component.variables[variableIndex].type;
									if(variableType==Component::Variable::Type::intermediate)
									{
										throw errorString(string()+"Variable '"+name+"' already used as output",lineIndex);
									}
									else if(variableType!=Component::Variable::Type::output && variableType!=Component::Variable::Type::reg)
									{
										throw errorString(string()+"Invalid variable type for use as output: '"+name+"'",lineIndex);
									}
									
									outputVariableIndexes.push_back(variableIndex);
								}
								else
								{
									outputVariableIndexes.push_back(component.variables.size());
									
									component.variables.emplace_back(Component::Variable::Type::intermediate,name,outputSizes[i],lineIndex);
								}
							}
							
							for(int i=0;i<outputVariableIndexes.size();i++)
							{
								int index=outputVariableIndexes[i];
								for(int j=i+1;j<outputVariableIndexes.size();j++)
								{
									if(index==outputVariableIndexes[j])
									{
										throw errorString(string()+"Repeated output variable '"+component.variables[index].name+"'",lineIndex);
									}
								}
							}
							
							assignmentLines.emplace_back(lineIndex,outputs,componentName,inputs,
								elementType,componentIndex,numberOfInstancesOfComponent,outputSizes,inputSizes,outputVariableIndexes);
						}
					}
					
					for(int assignmentLineIndex=0;assignmentLineIndex<assignmentLines.size();assignmentLineIndex++)
					{
						AssignmentLineData& assignmentLine=assignmentLines[assignmentLineIndex];
						
						int lineIndex=assignmentLine.lineIndex;
						
						vector<Component::Element::Output> elementOutputs;
						for(int i=0;i<assignmentLine.outputVariableIndexes.size();i++)
						{
							elementOutputs.emplace_back(assignmentLine.outputVariableIndexes[i]);
						}
						
						vector<Component::Element::Input> elementInputs;
						for(int i=0;i<assignmentLine.inputs.size();i++)
						{
							string str=assignmentLine.inputs[i];
							
							InputExpression inputExpression=parseInputExpression(str,component.variables,assignmentLine.inputSizes[i],assignmentLine.lineIndex);
							
							elementInputs.emplace_back(inputExpression);
						}
						
						for(int e=0;e<component.elements.size();e++)
						{
							Component::Element& element=component.elements[e];
							for(int i=0;i<element.outputs.size();i++)
							{
								for(int j=0;j<elementOutputs.size();j++)
								{
									if(elementOutputs[j].variableIndex==element.outputs[i].variableIndex)
									{
										string variableName=component.variables[elementOutputs[j].variableIndex].name;
										throw errorString(string()+"Variable used more than once as output: '"+variableName+"'",lineIndex);
									}
								}
							}
						}
						
						component.elements.emplace_back(
							assignmentLine.type,
							assignmentLine.type==Component::Element::Type::component ? assignmentLine.componentName : string(),
							assignmentLine.componentIndex,
							assignmentLine.numberOfInstancesOfComponent,
							elementOutputs,
							elementInputs,
							lineIndex
							);
					}
					
					for(int v=0;v<component.variables.size();v++)
					{
						const Component::Variable& variable=component.variables[v];
						
						if(variable.type==Component::Variable::Type::output || variable.type==Component::Variable::Type::reg)
						{
							bool found=false;
							
							for(int e=0;e<component.elements.size();e++)
							{
								Component::Element& element=component.elements[e];
								
								for(int j=0;j<element.outputs.size();j++)
								{
									if(element.outputs[j].variableIndex==v)
									{
										found=true;
										break;
									}
								}
								if(found) break;
							}
							
							if(!found) throw errorString(string()+"Variable '"+variable.name+"' without assignment set (undefined value)",variable.lineIndex);
						}
					}
				}
			}
			void checkRecursivity()
			{
				bool definable=true;
				vector<int> defined(components.size(),false);
				int definedCountOld=0;
				for(;;)
				{
					int definedCount=0;
					for(int i=0;i<components.size();i++)
					{
						if(!defined[i])
						{
							bool success=true;
							for(int j=0;j<components[i].elements.size();j++)
							{
								Component::Element& e=components[i].elements[j];
								if(e.type==Component::Element::Type::component)
								{
									if(!defined[e.componentIndex])
									{
										success=false;
										break;
									}
								}
							}
							if(success) defined[i]=true;
						}
						
						definedCount+=defined[i];
					}
					
					if(definedCount==defined.size()) break;
					else if(definedCount==definedCountOld)
					{
						definable=false;
						break;
					}
					
					definedCountOld=definedCount;
				}
				
				if(!definable)
				{
					string str="";
					int count=0;
					for(int i=0;i<components.size();i++)
					{
						if(!defined[i])
						{
							if(count>=20)
							{
								str+="...";
								break;
							}
							if(count>0) str+=", ";
							
							str+=components[i].name+" (line "+std::to_string(resolveLineIndex(components[i].firstLine)+1)+")";
							
							count++;
						}
					}
					
					throw string()+"Error: Cannot compile certain components due to infinite recursivity: "+str;
				}
			}
			
			class VariableData
			{
				public:
				
				vector<ComputerData::Pointer> computerInputs;
				vector<ComputerData::Pointer> computerMemoryInputs;
			};
			
			class ComponentContext
			{
				public:
				
				int level=0;
				
				vector<vector<ComputerData::Pointer>> inputs;
				
				vector<vector<ComputerData::Pointer>> outputs;
				
				vector<VariableData> variables;
				
				vector<int> lineIndexes;
				
				ComponentContext(){}
				ComponentContext(const ComponentContext& parent,const vector<vector<ComputerData::Pointer>>& _inputs,int elementLineIndex)
				{
					level=parent.level+1;
					
					inputs=_inputs;
					
					lineIndexes=parent.lineIndexes;
					lineIndexes.push_back(elementLineIndex);
				}
			};
			
			vector<ComputerData::Pointer> getComputerInputs(const Component& component,const ComponentContext& context,
				const Component::Element& element,const Component::Element::Input& elementInput)
			{
				vector<ComputerData::Pointer> computerInputs;
				
				InputExpressionEvaluator::BitOriginVector bitOriginVector=evaluateInputExpression(elementInput.expression.get(),element.lineIndex);
				
				for(int i=0;i<bitOriginVector.content.size();i++)
				{
					InputExpressionEvaluator::BitOrigin& bitOrigin=bitOriginVector.content[i];
					
					if(bitOrigin.type==InputExpressionEvaluator::BitOrigin::Type::constant)
					{
						int bit=(uint32_t(bitOrigin.constantValue))&1;
						computerInputs.emplace_back(ComputerData::Pointer::Type::constant,bit);
					}
					else if(bitOrigin.type==InputExpressionEvaluator::BitOrigin::Type::variable)
					{
						const Component::Variable& variable=component.variables[bitOrigin.variableIndex];
						const VariableData& variableData=context.variables[bitOrigin.variableIndex];
						
						if(variable.type==Component::Variable::Type::reg)
						{
							computerInputs.push_back(variableData.computerMemoryInputs[bitOrigin.variableBitIndex]);
						}
						else
						{
							computerInputs.push_back(variableData.computerInputs[bitOrigin.variableBitIndex]);
						}
					}
				}
				
				return computerInputs;
			}
			
			void setVariableInputs(ComputerData& computer,ComponentContext& context,int variableIndex,const vector<ComputerData::Pointer>& inputs)
			{
				vector<ComputerData::Pointer>& computerInputs=context.variables[variableIndex].computerInputs;
				for(int i=0;i<computerInputs.size();i++)
				{
					ComputerData::Pointer& input=computerInputs[i];
					
					if(input.type==ComputerData::Pointer::Type::yesGate)
					{
						computer.setYesGateInput(input.index,inputs[i]);
					}
					else
					{
						throw string("Internal error: written two times to the same variable (conflict)");
					}
				}
				
				computerInputs=inputs;
			}
			
			void addLineIndexesToNandGate(ComputerData& computer,int nandGateIndex,const vector<int>& lineIndexes)
			{
				computer.nandGates[nandGateIndex].lineIndexesIndex=computer.nandGates_lineIndexes_array.size();
				computer.nandGates_lineIndexes_array.push_back(lineIndexes);
			}
			
			void compileComponent(ComputerData& computer,int thisComponentIndex,ComponentContext& context)
			{
				Component& component=components[thisComponentIndex];
				
				for(int i=0,inputIndex=0;i<component.variables.size();i++)
				{
					const Component::Variable& variable=component.variables[i];
					
					{
						VariableData variableData;
						for(int j=0;j<variable.sizeInBits;j++)
						{
							variableData.computerInputs.emplace_back(ComputerData::Pointer::Type::yesGate,computer.addYesGate(ComputerData::YesGate()));
						}
						context.variables.push_back(variableData);
					}
					
					if(variable.type==Component::Variable::Type::input)
					{
						vector<ComputerData::Pointer> inputs;
						
						if(context.level==0)
						{
							int index=computer.addInputs(variable.sizeInBits);
							
							for(int j=0;j<variable.sizeInBits;j++)
							{
								inputs.emplace_back(ComputerData::Pointer::Type::input,index+j);
							}
						}
						else
						{
							inputs=context.inputs[inputIndex];
						}
						
						setVariableInputs(computer,context,i,inputs);
						
						inputIndex++;
					}
					else if(variable.type==Component::Variable::Type::reg)
					{
						vector<ComputerData::Pointer> inputs;
						
						for(int j=0;j<variable.sizeInBits;j++)
						{
							int index=computer.addMemory(ComputerData::Pointer());
							inputs.emplace_back(ComputerData::Pointer::Type::memory,index);
						}
						
						context.variables[i].computerMemoryInputs=inputs;
					}
				}
				
				for(int e=0;e<component.elements.size();e++)
				{
					Component::Element& element=component.elements[e];
					
					if(element.type==Component::Element::Type::nand)
					{
						vector<ComputerData::Pointer> inputsA=getComputerInputs(component,context,element,element.inputs[0]);
						vector<ComputerData::Pointer> inputsB=getComputerInputs(component,context,element,element.inputs[1]);
						
						vector<ComputerData::Pointer> inputsC;
						
						vector<int> lineIndexes=context.lineIndexes;
						lineIndexes.push_back(element.lineIndex);
						
						for(int i=0;i<inputsA.size() && i<inputsB.size();i++)
						{
							int nandGateIndex=computer.addNandGate(ComputerData::NandGate(inputsA[i],inputsB[i]));
							
							inputsC.emplace_back(ComputerData::Pointer::Type::nandGate,nandGateIndex);
							
							addLineIndexesToNandGate(computer,nandGateIndex,lineIndexes);
						}
						
						setVariableInputs(computer,context,element.outputs[0].variableIndex,inputsC);
					}
					else if(element.type==Component::Element::Type::set)
					{
						vector<ComputerData::Pointer> inputs=getComputerInputs(component,context,element,element.inputs[0]);
						
						setVariableInputs(computer,context,element.outputs[0].variableIndex,inputs);
					}
					else if(element.type==Component::Element::Type::concat)
					{
						vector<ComputerData::Pointer> inputsA=getComputerInputs(component,context,element,element.inputs[0]);
						vector<ComputerData::Pointer> inputsB=getComputerInputs(component,context,element,element.inputs[1]);
						
						vector<ComputerData::Pointer> inputsC;
						
						int inputsAwidth=inputsA.size()/element.numberOfInstances;
						int inputsBwidth=inputsB.size()/element.numberOfInstances;
						for(int i=0;i<element.numberOfInstances;i++)
						{
							inputsC.insert(inputsC.end(),inputsA.begin()+i*inputsAwidth,inputsA.begin()+(i+1)*inputsAwidth);
							inputsC.insert(inputsC.end(),inputsB.begin()+i*inputsBwidth,inputsB.begin()+(i+1)*inputsBwidth);
						}
						
						setVariableInputs(computer,context,element.outputs[0].variableIndex,inputsC);
					}
					else if(element.type==Component::Element::Type::component)
					{
						vector<vector<ComputerData::Pointer>> inputs;
						for(int i=0;i<element.inputs.size();i++)
						{
							inputs.push_back(getComputerInputs(component,context,element,element.inputs[i]));
						}
						
						vector<vector<ComputerData::Pointer>> outputs(element.outputs.size());
						
						for(int i=0;i<element.numberOfInstances;i++)
						{
							vector<vector<ComputerData::Pointer>> instanceInputs(inputs.size());
							for(int j=0;j<inputs.size();j++)
							{
								instanceInputs[j]=vector<ComputerData::Pointer>(
									inputs[j].begin()+i*(inputs[j].size()/element.numberOfInstances),
									inputs[j].begin()+(i+1)*(inputs[j].size()/element.numberOfInstances)
									);
							}
							
							ComponentContext subcontext(context,instanceInputs,element.lineIndex);
							
							compileComponent(computer,element.componentIndex,subcontext);
							
							for(int j=0;j<element.outputs.size();j++)
							{
								outputs[j].insert(outputs[j].end(),subcontext.outputs[j].begin(),subcontext.outputs[j].end());
							}
						}
						
						for(int i=0;i<element.outputs.size();i++)
						{
							setVariableInputs(computer,context,element.outputs[i].variableIndex,outputs[i]);
						}
					}
				}
				
				for(int i=0;i<component.variables.size();i++)
				{
					const Component::Variable& variable=component.variables[i];
					
					if(variable.type==Component::Variable::Type::reg)
					{
						for(int j=0;j<context.variables[i].computerInputs.size();j++)
						{
							computer.setMemory(context.variables[i].computerMemoryInputs[j].index,context.variables[i].computerInputs[j]);
						}
					}
					else if(variable.type==Component::Variable::Type::output)
					{
						vector<ComputerData::Pointer> inputs=context.variables[i].computerInputs;
						
						if(context.level==0)
						{
							for(int j=0;j<inputs.size();j++)
							{
								computer.addOutput(inputs[j]);
							}
						}
						
						context.outputs.push_back(inputs);
					}
				}
			}
			void redirectThroughYesGates(const ComputerData& computer,ComputerData::Pointer& input)
			{
				while(input.type==ComputerData::Pointer::Type::yesGate)
				{
					const ComputerData::YesGate& yesGate=computer.yesGates[input.index];
					if(!yesGate.inputResolved)
					{
						throw string("Internal error: yesGate not resolved");
					}
					input=yesGate.input;
				}
			}
			void optimizeYesGatesAway(ComputerData& computer)
			{
				for(int i=0;i<computer.nandGates.size();i++)
				{
					redirectThroughYesGates(computer,computer.nandGates[i].inputA);
					redirectThroughYesGates(computer,computer.nandGates[i].inputB);
				}
				for(int i=0;i<computer.memory.size();i++)
				{
					redirectThroughYesGates(computer,computer.memory[i].input);
				}
				for(int i=0;i<computer.outputs.size();i++)
				{
					redirectThroughYesGates(computer,computer.outputs[i].input);
				}
				computer.yesGates.resize(0);
			}
			void reorderNandGatesInValidOrder(ComputerData& computer)
			{
				vector<int> orderOldToNew(computer.nandGates.size(),-1);
				int orderedCount=0;
				
				for(;;)
				{
					int orderedCountOld=orderedCount;
					for(int i=0;i<computer.nandGates.size();i++)
					{
						if(orderOldToNew[i]==-1)
						{
							ComputerData::NandGate& nandGate=computer.nandGates[i];
							if(nandGate.inputA.type==ComputerData::Pointer::Type::nandGate)
							{
								if(orderOldToNew[nandGate.inputA.index]==-1) continue;
							}
							if(nandGate.inputB.type==ComputerData::Pointer::Type::nandGate)
							{
								if(orderOldToNew[nandGate.inputB.index]==-1) continue;
							}
							
							orderOldToNew[i]=orderedCount;
							orderedCount++;
						}
					}
					
					if(orderedCount==computer.nandGates.size()) break;
					else if(orderedCount==orderedCountOld)
					{
						string str;
						int count=0;
						vector<vector<int>> otherLineIndexes;
						for(int i=0;i<computer.nandGates.size();i++)
						{
							if(orderOldToNew[i]==-1)
							{
								int lineIndexesIndex=computer.nandGates[i].lineIndexesIndex;
								if(lineIndexesIndex==-1)
								{
									str+="Unknown lines\n";
									count++;
									continue;
								}
								
								vector<int> lineIndexes=computer.nandGates_lineIndexes_array[lineIndexesIndex];
								bool valid=true;
								for(int j=0;j<lineIndexes.size();j++)
								{
									int lineIndex=lineIndexes[j];
									if(lineIndex<0 || lineIndex>=lines.size())
									{
										valid=false;
										break;
									}
								}
								if(!valid)
								{
									str+="Unknown lines\n";
									count++;
									continue;
								}
								
								bool sameLines=false;
								for(int j=0;j<otherLineIndexes.size();j++)
								{
									if(otherLineIndexes[j].size()!=lineIndexes.size()) continue;
									bool equal=true;
									for(int k=0;k<lineIndexes.size();k++)
									{
										if(otherLineIndexes[j][k]!=lineIndexes[k])
										{
											equal=false;
											break;
										}
									}
									if(equal)
									{
										sameLines=true;
										break;
									}
								}
								if(sameLines) continue;
								otherLineIndexes.push_back(lineIndexes);
								
								if(count>=10)
								{
									str+="...";
									break;
								}
								
								str+="\n";
								
								for(int j=0;j<lineIndexes.size();j++)
								{
									int lineIndex=lineIndexes[j];
									int originalLineIndex=resolveLineIndex(lineIndex);
									
									if(j==0) str+="At ";
									else str+="from ";
									
									str+=std::to_string(originalLineIndex+1)+": "+originalLines[originalLineIndex];
									
									if(originalLines[originalLineIndex]!=lines[lineIndex])
									{
										str+=string("  ->  ")+lines[lineIndex];
									}
									
									str+="\n";
								}
								
								str+="\n";
								
								count++;
							}
						}
						throw string("Error: nand gate loops. Showing first nand gates with non resolvable inputs:\n")+str;
					}
				}
				
				vector<int> orderNewToOld(orderOldToNew.size());
				for(int i=0;i<orderOldToNew.size();i++)
				{
					orderNewToOld[orderOldToNew[i]]=i;
				}
				
				reorderNandGates(computer,orderNewToOld,orderOldToNew);
			}
			static void reorderNandGates(ComputerData& computer,const vector<int>& orderNewToOld,const vector<int>& orderOldToNew)
			{
				vector<ComputerData::NandGate> newNandGates(orderNewToOld.size());
				for(int i=0;i<orderNewToOld.size();i++)
				{
					ComputerData::NandGate nandGate=computer.nandGates[orderNewToOld[i]];
					
					if(nandGate.inputA.type==ComputerData::Pointer::Type::nandGate)
					{
						nandGate.inputA.index=orderOldToNew[nandGate.inputA.index];
					}
					if(nandGate.inputB.type==ComputerData::Pointer::Type::nandGate)
					{
						nandGate.inputB.index=orderOldToNew[nandGate.inputB.index];
					}
					
					newNandGates[i]=nandGate;
				}
				
				computer.nandGates=newNandGates;
				
				for(int i=0;i<computer.memory.size();i++)
				{
					ComputerData::OutputGate& memory=computer.memory[i];
					
					if(memory.input.type==ComputerData::Pointer::Type::nandGate)
					{
						memory.input.index=orderOldToNew[memory.input.index];
					}
				}
				for(int i=0;i<computer.outputs.size();i++)
				{
					ComputerData::OutputGate& output=computer.outputs[i];
					
					if(output.input.type==ComputerData::Pointer::Type::nandGate)
					{
						output.input.index=orderOldToNew[output.input.index];
					}
				}
			}
			static void reorderMemory(ComputerData& computer,const vector<int>& orderNewToOld,const vector<int>& orderOldToNew)
			{
				vector<ComputerData::OutputGate> newMemory(orderNewToOld.size());
				for(int i=0;i<orderNewToOld.size();i++)
				{
					ComputerData::OutputGate memory=computer.memory[orderNewToOld[i]];
					
					if(memory.input.type==ComputerData::Pointer::Type::memory)
					{
						memory.input.index=orderOldToNew[memory.input.index];
					}
					
					newMemory[i]=memory;
				}
				
				computer.memory=newMemory;
				
				for(int i=0;i<computer.nandGates.size();i++)
				{
					ComputerData::NandGate& nandGate=computer.nandGates[i];
					
					if(nandGate.inputA.type==ComputerData::Pointer::Type::memory)
					{
						nandGate.inputA.index=orderOldToNew[nandGate.inputA.index];
					}
					if(nandGate.inputB.type==ComputerData::Pointer::Type::memory)
					{
						nandGate.inputB.index=orderOldToNew[nandGate.inputB.index];
					}
				}
				for(int i=0;i<computer.outputs.size();i++)
				{
					ComputerData::OutputGate& output=computer.outputs[i];
					
					if(output.input.type==ComputerData::Pointer::Type::memory)
					{
						output.input.index=orderOldToNew[output.input.index];
					}
				}
			}
		public:
		
		static int pruneUnusedNandGates(ComputerData& computer)
		{
			vector<int> used(computer.nandGates.size(),false);
			int usedCount=0;
			
			for(int i=0;i<computer.memory.size();i++)
			{
				ComputerData::OutputGate& memory=computer.memory[i];
				
				if(memory.input.type==ComputerData::Pointer::Type::nandGate)
				{
					int index=memory.input.index;
					if(!used[index])
					{
						used[index]=true;
						usedCount++;
					}
				}
			}
			for(int i=0;i<computer.outputs.size();i++)
			{
				ComputerData::OutputGate& output=computer.outputs[i];
				
				if(output.input.type==ComputerData::Pointer::Type::nandGate)
				{
					int index=output.input.index;
					if(!used[index])
					{
						used[index]=true;
						usedCount++;
					}
				}
			}
			
			while(usedCount<used.size())
			{
				int usedCountOld=usedCount;
				
				for(int i=0;i<computer.nandGates.size();i++)
				{
					if(used[i])
					{
						ComputerData::NandGate& nandGate=computer.nandGates[i];
						if(nandGate.inputA.type==ComputerData::Pointer::Type::nandGate)
						{
							int index=nandGate.inputA.index;
							if(!used[index])
							{
								used[index]=true;
								usedCount++;
							}
						}
						if(nandGate.inputB.type==ComputerData::Pointer::Type::nandGate)
						{
							int index=nandGate.inputB.index;
							if(!used[index])
							{
								used[index]=true;
								usedCount++;
							}
						}
					}
				}
				
				if(usedCount==usedCountOld) break;
			}
			
			vector<int> orderNewToOld(usedCount);
			vector<int> orderOldToNew(used.size());
			for(int i=0,count=0;i<used.size();i++)
			{
				if(used[i])
				{
					orderOldToNew[i]=count;
					orderNewToOld[count]=i;
					count++;
				}
			}
			
			reorderNandGates(computer,orderNewToOld,orderOldToNew);
			
			return used.size()-usedCount;
		}
		static int pruneUnusedMemory(ComputerData& computer)
		{
			vector<int> used(computer.memory.size(),false);
			int usedCount=0;
			
			for(int i=0;i<computer.memory.size();i++)
			{
				ComputerData::OutputGate& memory=computer.memory[i];
				
				if(memory.input.type==ComputerData::Pointer::Type::memory)
				{
					int index=memory.input.index;
					if(!used[index])
					{
						used[index]=true;
						usedCount++;
					}
				}
			}
			for(int i=0;i<computer.outputs.size();i++)
			{
				ComputerData::OutputGate& output=computer.outputs[i];
				
				if(output.input.type==ComputerData::Pointer::Type::memory)
				{
					int index=output.input.index;
					if(!used[index])
					{
						used[index]=true;
						usedCount++;
					}
				}
			}
			for(int i=0;i<computer.nandGates.size();i++)
			{
				ComputerData::NandGate& nandGate=computer.nandGates[i];
				if(nandGate.inputA.type==ComputerData::Pointer::Type::memory)
				{
					int index=nandGate.inputA.index;
					if(!used[index])
					{
						used[index]=true;
						usedCount++;
					}
				}
				if(nandGate.inputB.type==ComputerData::Pointer::Type::memory)
				{
					int index=nandGate.inputB.index;
					if(!used[index])
					{
						used[index]=true;
						usedCount++;
					}
				}
			}
			
			vector<int> orderNewToOld(usedCount);
			vector<int> orderOldToNew(used.size());
			for(int i=0,count=0;i<used.size();i++)
			{
				if(used[i])
				{
					orderOldToNew[i]=count;
					orderNewToOld[count]=i;
					count++;
				}
			}
			
			reorderMemory(computer,orderNewToOld,orderOldToNew);
			
			return used.size()-usedCount;
		}
		
		private:
			void compileComponents(ComputerData& computer,const string& mainComponentName="main")
			{
				int mainComponentIndex=findWithName(components,mainComponentName);
				if(mainComponentIndex==-1) throw string()+"Error: '"+mainComponentName+"' component not found";
				
				ComponentContext context;
				compileComponent(computer,mainComponentIndex,context);
				
				optimizeYesGatesAway(computer);
				reorderNandGatesInValidOrder(computer);
				computer.removeLineIndexData();
			}
			
			class Optimizer
			{
				private:
					class ConnectionChange
					{
						public:
						
						ComputerData::Pointer oldInputComponent;
						ComputerData::Pointer newInputComponent;
						int outputComponentInputIndex=0;
						ComputerData::Pointer outputComponent;
						
						ConnectionChange(){}
						ConnectionChange(const ComputerData::Pointer& _oldInputComponent,const ComputerData::Pointer& _newInputComponent,
							int _outputComponentInputIndex,const ComputerData::Pointer& _outputComponent)
						{
							oldInputComponent=_oldInputComponent;
							newInputComponent=_newInputComponent;
							outputComponentInputIndex=_outputComponentInputIndex;
							outputComponent=_outputComponent;
						}
					};
					
					class ComputerOutputData
					{
						public:
						
						static constexpr bool checkInRange=true;
						
						vector<vector<ComputerData::Pointer>> inputs;
						vector<vector<ComputerData::Pointer>> memory;
						vector<vector<ComputerData::Pointer>> nandGates;
						
						vector<ComputerData::Pointer> getOutputs(const ComputerData::Pointer& component)
						{
							vector<ComputerData::Pointer>*outputsPtr=getOutputsPtr(component);
							
							if(outputsPtr==nullptr) return vector<ComputerData::Pointer>();
							
							return *outputsPtr;
						}
						vector<ComputerData::Pointer>*getOutputsPtr(const ComputerData::Pointer& component)
						{
							vector<vector<ComputerData::Pointer>>*vectorPtr=getVectorPtr(component);
							
							if(vectorPtr==nullptr) return nullptr;
							if(checkInRange)
							{
								if(component.index<0 || component.index>=vectorPtr->size()) throw string()+"Getting outputs of a component out of range";
							}
							
							return &(*vectorPtr)[component.index];
						}
						private:
							vector<vector<ComputerData::Pointer>>*getVectorPtr(const ComputerData::Pointer& component)
							{
								vector<vector<ComputerData::Pointer>>*vectorPtr=nullptr;
								
								if(component.type==ComputerData::Pointer::Type::input)
								{
									vectorPtr=&inputs;
								}
								else if(component.type==ComputerData::Pointer::Type::memory)
								{
									vectorPtr=&memory;
								}
								else if(component.type==ComputerData::Pointer::Type::nandGate)
								{
									vectorPtr=&nandGates;
								}
								
								return vectorPtr;
							}
						public:
						void addOutput(const ComputerData::Pointer& component,const ComputerData::Pointer& output)
						{
							vector<vector<ComputerData::Pointer>>*vectorPtr=getVectorPtr(component);
							
							if(vectorPtr==nullptr) throw string()+"Adding output to a component with no outputs allowed";
							if(component.index<0 || component.index>=vectorPtr->size()) throw string()+"Adding output to a component out of range";
							
							(*vectorPtr)[component.index].push_back(output);
						}
						void removeOutput(const ComputerData::Pointer& component,const ComputerData::Pointer& output)
						{
							vector<vector<ComputerData::Pointer>>*vectorPtr=getVectorPtr(component);
							
							if(vectorPtr==nullptr) throw string()+"Removing output from a component with no outputs allowed";
							if(component.index<0 || component.index>=vectorPtr->size()) throw string()+"Removing output from a component out of range";
							
							vector<ComputerData::Pointer>& v=(*vectorPtr)[component.index];
							
							for(int i=0;i<v.size();i++)
							{
								if(v[i]==output)
								{
									v.erase(v.begin()+i);
									break;
								}
							}
						}
						void change(const ConnectionChange& connectionChange)
						{
							if(connectionChange.outputComponent.type!=ComputerData::Pointer::Type::memory)
							{
								if(connectionChange.newInputComponent==connectionChange.outputComponent
									|| connectionChange.oldInputComponent==connectionChange.outputComponent) throw string()+"Input and output are the same component";
							}
							
							if(connectionChange.newInputComponent!=connectionChange.oldInputComponent)
							{
								if(connectionChange.oldInputComponent.type!=ComputerData::Pointer::Type::constant)
								{
									removeOutput(connectionChange.oldInputComponent,connectionChange.outputComponent);
								}
								
								if(connectionChange.newInputComponent.type!=ComputerData::Pointer::Type::constant)
								{
									addOutput(connectionChange.newInputComponent,connectionChange.outputComponent);
								}
							}
						}
						void update(ComputerData& computer)
						{
							inputs=vector<vector<ComputerData::Pointer>>(computer.numberOfInputs);
							memory=vector<vector<ComputerData::Pointer>>(computer.memory.size());
							nandGates=vector<vector<ComputerData::Pointer>>(computer.nandGates.size());
							
							vector<ComputerData::Pointer> outputComponents;
							for(int i=0;i<computer.outputs.size();i++)
							{
								outputComponents.emplace_back(ComputerData::Pointer::Type::output,i);
							}
							for(int i=0;i<computer.memory.size();i++)
							{
								outputComponents.emplace_back(ComputerData::Pointer::Type::memory,i);
							}
							for(int i=0;i<computer.nandGates.size();i++)
							{
								outputComponents.emplace_back(ComputerData::Pointer::Type::nandGate,i);
							}
							
							for(int i=0;i<outputComponents.size();i++)
							{
								vector<ComputerData::Pointer> inputs=outputComponents[i].getInputsDistinct(computer);
								for(int j=0;j<inputs.size();j++)
								{
									if(inputs[j].type!=ComputerData::Pointer::Type::constant)
									{
										addOutput(inputs[j],outputComponents[i]);
									}
								}
							}
						}
						vector<ComputerData::Pointer> getAllPointers()
						{
							vector<ComputerData::Pointer> pointers;
							for(int i=0;i<inputs.size();i++)
							{
								pointers.emplace_back(ComputerData::Pointer::Type::input,i);
							}
							for(int i=0;i<memory.size();i++)
							{
								pointers.emplace_back(ComputerData::Pointer::Type::memory,i);
							}
							for(int i=0;i<nandGates.size();i++)
							{
								pointers.emplace_back(ComputerData::Pointer::Type::nandGate,i);
							}
							return pointers;
						}
					};
					
					class Rule
					{
						public:
						
						string name;
						
						ComputerData pattern;
						ComputerOutputData patternOutputData;
						
						ComputerData::Pointer patternMainOutput;
						
						ComputerData resultingPattern;
						
						Rule(){}
						Rule(const string& _name,const ComputerData& _pattern,const ComputerData& _resultingPattern)
						{
							name=_name;
							pattern=_pattern;
							resultingPattern=_resultingPattern;
							
							if(pattern.numberOfInputs!=resultingPattern.numberOfInputs) throw string()+"Number of inputs do not match";
							if(pattern.outputs.size()!=resultingPattern.outputs.size()) throw string()+"Number of outputs do not match";
							if(pattern.memory.size()!=resultingPattern.memory.size()) throw string()+"Number of memory bits do not match";
							if(resultingPattern.nandGates.size()>pattern.nandGates.size())
							{
								throw string()+"The result of the pattern has more nand gates than the pattern";
							}
							if(pattern.outputs.size()+pattern.memory.size()+pattern.nandGates.size()==0)
							{
								throw string()+"The pattern has no outputs or gates or memory";
							}
							if(pattern.outputs.size()==0)
							{
								throw string()+"The pattern has no outputs";
							}
							
							patternOutputData.update(pattern);
							
							if(hasNonOutputComponentWithNoOutputs(patternOutputData))
							{
								throw string()+"The pattern has non output components with no output (unused components)";
							}
							if(hasDuplicatedOutputOutputs(patternOutputData))
							{
								throw string()+"The pattern has more than one pattern output with the same input";
							}
							
							patternMainOutput=chooseMainOutput(pattern,patternOutputData);
							
							if(!isSingleConnectedNetwork(pattern,patternOutputData,patternMainOutput))
							{
								throw string()+"The pattern is not a single connected network";
							}
						}
						
						private:
							template <class T>
							class Stack
							{
								private:
									vector<T> elements;
									size_t internal_size=0;
									
									vector<size_t> internal_baseSize;
									
									static constexpr int growthFactor=2;
									
									T*push(size_t _size)
									{
										size_t newSize=internal_size+_size;
										if(newSize>elements.size()) reserve(newSize>internal_size*growthFactor ? newSize : internal_size*growthFactor);
										size_t offset=internal_size;
										internal_size=newSize;
										return &elements[offset];
									}
								public:
								
								class Slice
								{
									private:
										Stack*internal_stackPtr=nullptr;
										T*internal_elementPtr=nullptr;
										size_t internal_size=0;
									public:
									
									void push(Stack& stack,size_t _size)
									{
										if(internal_stackPtr!=nullptr)
										{
											throw string()+"Trying to push a slice that was already pushed";
										}
										internal_stackPtr=&stack;
										internal_elementPtr=stack.push(_size);
										internal_size=_size;
									}
									
									Slice(){}
									Slice(const Slice& other)=delete;
									
									void operator =(const Slice& other)
									{
										if(internal_stackPtr==nullptr)
										{
											throw string()+"Assigning a slice to a slice with no stack";
										}
										if(other.internal_stackPtr==nullptr)
										{
											throw string()+"Assigning a slice with no stack to another slice";
										}
										if(internal_size!=other.internal_size)
										{
											throw string()+"Assigning a slice to another slice with different size";
										}
										for(size_t i=0;i<internal_size;i++)
										{
											internal_elementPtr[i]=other.internal_elementPtr[i];
										}
									}
									
									explicit operator vector<T>() const
									{
										vector<T> v(internal_size);
										for(size_t i=0;i<internal_size;i++)
										{
											v[i]=internal_elementPtr[i];
										}
										return v;
									}
									
									inline size_t size() const
									{
										return internal_size;
									}
									inline T& operator [](size_t index)
									{
										return internal_elementPtr[index];
									}
									inline const T& operator [](size_t index) const
									{
										return internal_elementPtr[index];
									}
								};
								
								Stack(){}
								Stack(size_t sizeToReserve)
								{
									reserve(sizeToReserve);
								}
								
								void pushBase()
								{
									internal_baseSize.push_back(internal_size);
								}
								void popBase()
								{
									if(internal_baseSize.size()>0)
									{
										internal_size=internal_baseSize.back();
										internal_baseSize.resize(internal_baseSize.size()-1);
									}
									else internal_size=0;
								}
								
								size_t size() const
								{
									return internal_size;
								}
								void reserve(size_t size)
								{
									elements.resize(size);
								}
								void reserveBase(size_t size)
								{
									internal_baseSize.reserve(size);
								}
							};
							
							template <class T>
							class StackFrame
							{
								private:
									T*stackPtr=nullptr;
								public:
								
								StackFrame(T& stack)
								{
									stack.pushBase();
									stackPtr=&stack;
								}
								~StackFrame()
								{
									stackPtr->popBase();
								}
							};
							
							using PointerSlice=Stack<ComputerData::Pointer>::Slice;
							
							class ComponentPointerStack
							{
								public:
								
								Stack<ComputerData::Pointer> stack;
								
								int inputSize=0;
								int outputSize=0;
								int memorySize=0;
								int nandSize=0;
								
								ComponentPointerStack(ComputerData& computer,int levels)
								{
									inputSize=computer.numberOfInputs;
									outputSize=computer.outputs.size();
									memorySize=computer.memory.size();
									nandSize=computer.nandGates.size();
									
									stack.reserve((inputSize+outputSize+memorySize+nandSize)*levels);
									stack.reserveBase(levels);
								}
								
								void pushSlices(PointerSlice& inputSlice,PointerSlice& outputSlice,PointerSlice& memorySlice,PointerSlice& nandSlice)
								{
									inputSlice.push(stack,inputSize);
									outputSlice.push(stack,outputSize);
									memorySlice.push(stack,memorySize);
									nandSlice.push(stack,nandSize);
								}
								
								void pushBase()
								{
									stack.pushBase();
								}
								void popBase()
								{
									stack.popBase();
								}
							};
							
							class ComponentPointers
							{
								private:
									PointerSlice input;
									PointerSlice output;
									PointerSlice memory;
									PointerSlice nand;
								public:
								
								static constexpr bool checkInRange=true;
								
								ComponentPointers(ComponentPointerStack& stack)
								{
									stack.pushSlices(input,output,memory,nand);
								}
								
								vector<ComputerData::Pointer> getInputs() const
								{
									return vector<ComputerData::Pointer>(input);
								}
								vector<ComputerData::Pointer> getOutputs() const
								{
									vector<ComputerData::Pointer> outputResult=vector<ComputerData::Pointer>(output);
									for(int i=0;i<outputResult.size();i++)
									{
										if(outputResult[i].type!=ComputerData::Pointer::Type::output)
										{
											outputResult[i]=ComputerData::Pointer();
										}
									}
									return outputResult;
								}
								vector<ComputerData::Pointer> getMemory() const
								{
									return vector<ComputerData::Pointer>(memory);
								}
								vector<ComputerData::Pointer> getNandGates() const
								{
									return vector<ComputerData::Pointer>(nand);
								}
								vector<ComputerData::Pointer> getOutputsRedirectedInPattern() const
								{
									vector<ComputerData::Pointer> boundOutput=vector<ComputerData::Pointer>(output);
									for(int i=0;i<boundOutput.size();i++)
									{
										if(boundOutput[i].type==ComputerData::Pointer::Type::output)
										{
											boundOutput[i]=ComputerData::Pointer();
										}
									}
									return boundOutput;
								}
								
								bool isAlreadyFound(const ComputerData::Pointer& pointerInPattern)
								{
									ComputerData::Pointer*ptr=getPtr(pointerInPattern);
									if(ptr==nullptr) return false;
									return !ptr->isNone();
								}
								bool isAlreadyFoundAs(const ComputerData::Pointer& pointerInPattern,const ComputerData::Pointer& pointerInComputer)
								{
									if(pointerInComputer.isNone()) return false;
									ComputerData::Pointer*ptr=getPtr(pointerInPattern);
									if(ptr==nullptr) return false;
									return *ptr==pointerInComputer;
								}
								
								bool add(const ComputerData::Pointer& pointerInPattern,const ComputerData::Pointer& pointerInComputer)
								{
									ComputerData::Pointer*ptr=getPtr(pointerInPattern);
									if(ptr==nullptr) return false;
									if(!ptr->isNone()) return false;
									*ptr=pointerInComputer;
									return true;
								}
								
								bool bindToOutput(const ComputerData::Pointer& pointerInPattern,const ComputerData::Pointer& outputPointerInPattern)
								{
									if(checkInRange)
									{
										if(outputPointerInPattern.type!=ComputerData::Pointer::Type::output) return false;
										if(!isInRange(outputPointerInPattern)) return false;
									}
									
									if(!isAlreadyFound(pointerInPattern)) return false;
									
									if(!output[outputPointerInPattern.index].isNone()) return false;
									output[outputPointerInPattern.index]=pointerInPattern;
									
									return true;
								}
								
								ComputerData::Pointer getBoundToOutput(const ComputerData::Pointer& outputPointerInPattern)
								{
									if(checkInRange)
									{
										if(outputPointerInPattern.type!=ComputerData::Pointer::Type::output)
										{
											return ComputerData::Pointer();
										}
										
										if(!isInRange(outputPointerInPattern)) return ComputerData::Pointer();
									}
									
									if(output[outputPointerInPattern.index].type==ComputerData::Pointer::Type::output)
									{
										return ComputerData::Pointer();
									}
									return output[outputPointerInPattern.index];
								}
								
								bool foundAll()
								{
									return foundAll(input) && foundAll(output) && foundAll(memory) && foundAll(nand);
								}
								ComputerData::Pointer translate(const ComputerData::Pointer& pointerInPattern)
								{
									ComputerData::Pointer*ptr=getPtr(pointerInPattern);
									if(ptr==nullptr) return pointerInPattern;
									return *ptr;
								}
								bool contains(const ComputerData::Pointer& pointerInComputer)
								{
									if(pointerInComputer.isNone()) return false;
									if(pointerInComputer.type==ComputerData::Pointer::Type::constant) return false;
									
									for(int i=0;i<input.size();i++)
									{
										if(input[i]==pointerInComputer) return true;
									}
									for(int i=0;i<output.size();i++)
									{
										if(output[i]==pointerInComputer) return true;
									}
									for(int i=0;i<memory.size();i++)
									{
										if(memory[i]==pointerInComputer) return true;
									}
									for(int i=0;i<nand.size();i++)
									{
										if(nand[i]==pointerInComputer) return true;
									}
									
									return false;
								}
								
								private:
									PointerSlice*getVectorPtr(const ComputerData::Pointer& pointer)
									{
										if(pointer.type==ComputerData::Pointer::Type::input)
										{
											return &input;
										}
										else if(pointer.type==ComputerData::Pointer::Type::output)
										{
											return &output;
										}
										else if(pointer.type==ComputerData::Pointer::Type::memory)
										{
											return &memory;
										}
										else if(pointer.type==ComputerData::Pointer::Type::nandGate)
										{
											return &nand;
										}
										else return nullptr;
									}
									ComputerData::Pointer*getPtr(const ComputerData::Pointer& pointerInPattern)
									{
										PointerSlice*vectorPtr=getVectorPtr(pointerInPattern);
										if(vectorPtr==nullptr) return nullptr;
										if(checkInRange)
										{
											if(pointerInPattern.index<0 || pointerInPattern.index>=vectorPtr->size())
											{
												return nullptr;
											}
										}
										return &(*vectorPtr)[pointerInPattern.index];
									}
									bool isInRange(const ComputerData::Pointer& pointer)
									{
										if(pointer.type==ComputerData::Pointer::Type::constant) return pointer.index>=0 && pointer.index<=1;
										
										PointerSlice*vectorPtr=getVectorPtr(pointer);
										if(vectorPtr==nullptr) return false;
										
										return pointer.index>=0 && pointer.index<vectorPtr->size();
									}
									bool foundAll(const PointerSlice& pointers)
									{
										for(int i=0;i<pointers.size();i++)
										{
											if(pointers[i].isNone()) return false;
										}
										return true;
									}
								public:
							};
							
							void isSingleConnectedNetwork_recursive(ComputerData& computer,ComputerOutputData& computerOutputData,
								const ComputerData::Pointer& component,ComponentPointers& pointers)
							{
								if(!pointers.add(component,component)) return;
								
								vector<ComputerData::Pointer> inputs=component.getInputs(computer);
								for(int i=0;i<inputs.size();i++)
								{
									isSingleConnectedNetwork_recursive(computer,computerOutputData,inputs[i],pointers);
								}
								
								vector<ComputerData::Pointer> outputs=computerOutputData.getOutputs(component);
								for(int i=0;i<outputs.size();i++)
								{
									isSingleConnectedNetwork_recursive(computer,computerOutputData,outputs[i],pointers);
								}
							}
							bool isSingleConnectedNetwork(ComputerData& computer,ComputerOutputData& computerOutputData,const ComputerData::Pointer& mainOutput)
							{
								ComponentPointerStack stack(computer,1);
								ComponentPointers pointers(stack);
								
								isSingleConnectedNetwork_recursive(computer,computerOutputData,mainOutput,pointers);
								
								return pointers.foundAll();
							}
							bool hasDuplicatedOutputOutputs(ComputerOutputData& computerOutputData)
							{
								vector<ComputerData::Pointer> pointers=computerOutputData.getAllPointers();
								for(int i=0;i<pointers.size();i++)
								{
									vector<ComputerData::Pointer> outputs=computerOutputData.getOutputs(pointers[i]);
									for(int j=0;j<outputs.size();j++)
									{
										for(int k=0;k<j;k++)
										{
											if(outputs[j]==outputs[k])
											{
												return true;
											}
										}
									}
								}
								return false;
							}
							bool hasNonOutputComponentWithNoOutputs(ComputerOutputData& computerOutputData)
							{
								vector<ComputerData::Pointer> pointers=computerOutputData.getAllPointers();
								for(int i=0;i<pointers.size();i++)
								{
									vector<ComputerData::Pointer> outputs=computerOutputData.getOutputs(pointers[i]);
									if(outputs.size()==0) return true;
								}
								return false;
							}
							
							vector<ComputerData::Pointer> getOutputs(ComputerOutputData& computerOutputData,const ComputerData::Pointer& component)
							{
								return computerOutputData.getOutputs(component);
							}
							vector<ComputerData::Pointer>*getOutputsPtr(ComputerOutputData& computerOutputData,const ComputerData::Pointer& component)
							{
								return computerOutputData.getOutputsPtr(component);
							}
							vector<ComputerData::Pointer> getInputsCanRepeat(ComputerData& computer,const ComputerData::Pointer& component)
							{
								vector<ComputerData::Pointer> inputs=component.getInputs(computer);
								
								return inputs;
							}
							bool addFoundComponent(const ComputerData::Pointer& componentInPattern,const ComputerData::Pointer& componentInComputer,
								ComponentPointers& pointers)
							{
								if(pointers.contains(componentInComputer)) return false;
								
								return pointers.add(componentInPattern,componentInComputer);
							}
							vector<ComputerData::Pointer> removeFoundPatternComponents(vector<ComputerData::Pointer>& componentsInPattern,ComponentPointers& pointers,
								const ComputerData::Pointer& excludeInOutput)
							{
								vector<ComputerData::Pointer> removed;
								removed.reserve(componentsInPattern.size());
								for(int i=0;i<componentsInPattern.size();i++)
								{
									if(componentsInPattern[i].type!=ComputerData::Pointer::Type::constant
										&& pointers.isAlreadyFound(componentsInPattern[i]))
									{
										if(componentsInPattern[i]!=excludeInOutput) removed.push_back(componentsInPattern[i]);
										componentsInPattern.erase(componentsInPattern.begin()+i);
										i--;
									}
								}
								return removed;
							}
							vector<ComputerData::Pointer> removeFoundComputerComponents(vector<ComputerData::Pointer>& componentsInComputer,ComponentPointers& pointers)
							{
								vector<ComputerData::Pointer> removed;
								removed.reserve(componentsInComputer.size());
								for(int i=0;i<componentsInComputer.size();i++)
								{
									if(pointers.contains(componentsInComputer[i]))
									{
										removed.push_back(componentsInComputer[i]);
										componentsInComputer.erase(componentsInComputer.begin()+i);
										i--;
									}
								}
								return removed;
							}
							bool removedFoundComponentsAreTheSame(
								const vector<ComputerData::Pointer>& componentsInPattern,
								const vector<ComputerData::Pointer>& componentsInComputer,
								ComponentPointers& pointers)
							{
								if(componentsInPattern.size()!=componentsInComputer.size()) return false;
								for(int i=0;i<componentsInPattern.size();i++)
								{
									bool found=false;
									for(int j=0;j<componentsInComputer.size();j++)
									{
										if(pointers.isAlreadyFoundAs(componentsInPattern[i],componentsInComputer[i]))
										{
											found=true;
											break;
										}
									}
									if(!found) return false;
								}
								return true;
							}
							bool isComponentAlreadyFoundAndIsItTheSame(const ComputerData::Pointer& componentInPattern,const ComputerData::Pointer& componentInComputer,
								ComponentPointers& pointers)
							{
								return pointers.isAlreadyFoundAs(componentInPattern,componentInComputer);
							}
							int factorial(int x,int stop)
							{
								try
								{
									if(x<=stop) return 1;
									else return (SafeInteger(x)*SafeInteger(factorial(x-1,stop))).getValue();
								}
								catch(const string& str)
								{
									throw string()+"Integer overflow in factorial: "+str;
								}
							}
							bool checkTooManyCombinations(int numberOfCombinations,int maxCombinations)
							{
								if(numberOfCombinations>maxCombinations)
								{
									addToMessageQueue("Too many combinations",true,
										string()+"Too many combinations ("+std::to_string(numberOfCombinations)+">"+std::to_string(maxCombinations)+")");
									return true;
								}
								else return false;
							}
							vector<vector<int>> getCombinations(int size,int possibleValues,int maxCombinations,bool& tooManyCombinations)
							{
								tooManyCombinations=false;
								if(possibleValues<size || size<0) throw string()+"No combinations possible";
								
								if(size==0) return vector<vector<int>>{vector<int>()};
								else if(size==1)
								{
									int numberOfCombinations=possibleValues;
									if(checkTooManyCombinations(numberOfCombinations,maxCombinations))
									{
										tooManyCombinations=true;
										return vector<vector<int>>();
									}
									
									vector<vector<int>> combinations(1,vector<int>(possibleValues));
									for(int i=0;i<combinations.size();i++)
									{
										combinations[0][i]=i;
									}
									return combinations;
								}
								else if(size==2)
								{
									int numberOfCombinations=possibleValues*(possibleValues-1);
									if(checkTooManyCombinations(numberOfCombinations,maxCombinations))
									{
										tooManyCombinations=true;
										return vector<vector<int>>();
									}
									
									vector<vector<int>> combinations(2,vector<int>(numberOfCombinations));
									for(int i=0;i<combinations.size();i++)
									{
										int valueA=i/(possibleValues-1);
										int valueB=i%(possibleValues-1);
										if(valueB>=valueA) valueB++;
										combinations[0][i]=valueA;
										combinations[1][i]=valueB;
									}
									return combinations;
								}
								else
								{
									int numberOfCombinations=0;
									try
									{
										numberOfCombinations=factorial(possibleValues,possibleValues-size);
									}
									catch(const string& str)
									{
										std::cout<<(string()+"Too many combinations : "+str)<<std::endl;
										tooManyCombinations=true;
										return vector<vector<int>>();
									}
									if(checkTooManyCombinations(numberOfCombinations,maxCombinations))
									{
										tooManyCombinations=true;
										return vector<vector<int>>();
									}
									
									vector<vector<int>> combinations(size,vector<int>(numberOfCombinations));
									vector<int> combination(size,0);
									vector<int> finalCombination(size,0);
									for(int c=0;c<numberOfCombinations;c++)
									{
										if(c>0)
										{
											for(int i=size-1;i>=0;i--)
											{
												combination[i]++;
												
												if(combination[i]>=possibleValues-i)
												{
													combination[i]=0;
												}
												else break;
											}
										}
										{
											finalCombination=combination;
											int lastMinimum=-1;
											for(int t=0;t<size-1;t++)
											{
												int minimum=possibleValues;
												int minimumIndex=0;
												for(int i=0;i<size;i++)
												{
													int value=finalCombination[i];
													if(value<minimum && value>lastMinimum)
													{
														minimum=value;
														minimumIndex=i;
													}
												}
												for(int i=minimumIndex+1;i<size;i++)
												{
													if(finalCombination[i]>=minimum) finalCombination[i]++;
												}
												lastMinimum=minimum;
											}
										}
										for(int i=0;i<size;i++)
										{
											combinations[i][c]=finalCombination[i];
										}
									}
									return combinations;
								}
							}
							int divideMaxCombinations(int maxCombinations,int divisor)
							{
								if(divisor<=0) throw string()+"Dividing maximum combination number by an integer less than 1";
								return maxCombinations/divisor+maxCombinations%divisor;
							}
							bool matchesWithCombinations(ComputerData& computer,ComputerOutputData& computerOutputData,
								const vector<ComputerData::Pointer>& computerComponents,
								const vector<ComputerData::Pointer>& patternComponents,
								const ComponentPointers& inputPointers,ComponentPointers& outputPointers,int maxCombinations,bool directionToOutput,
								ComponentPointerStack& stack,bool doCheckThis,bool doCheckInputs,bool doCheckOutputs,bool traverseInputs)
							{
								StackFrame stackFrame(stack);
								
								outputPointers=inputPointers;
								
								bool tooManyCombinations=false;
								vector<vector<int>> combinations=getCombinations(patternComponents.size(),computerComponents.size(),maxCombinations,tooManyCombinations);
								if(tooManyCombinations) return false;
								
								if(combinations.size()==0) throw string()+"Combination vector is empty";
								int numberOfCombinations=combinations[0].size();
								
								if(debugMode)
								{
									std::cout<<debugsc<<"Combinations (choose "<<patternComponents.size()<<" from "<<computerComponents.size()<<")("
										<<numberOfCombinations<<"):"<<std::endl;
									for(int j=0;j<numberOfCombinations;j++)
									{
										for(int i=0;i<combinations.size();i++)
										{
											if(i>0) std::cout<<",";
											else std::cout<<debugsc;
											std::cout<<combinations[i][j];
										}
										std::cout<<std::endl;
									}
									std::cout<<std::endl;
								}
								
								int maxCombinationsPerCombination=divideMaxCombinations(maxCombinations,numberOfCombinations);
								
								ComponentPointers outputPointersCombination(stack);
								ComponentPointers outputPointersCombinationNew(stack);
								
								bool combinationFound=false;
								for(int combination=0;combination<numberOfCombinations;combination++)
								{
									outputPointersCombination=outputPointers;
									
									bool combinationWorks=true;
									for(int i=0;i<patternComponents.size();i++)
									{
										debugscPut();
										
										if(matchesWith(computer,computerOutputData,computerComponents[combinations[i][combination]],
											patternComponents[i],
											outputPointersCombination,outputPointersCombinationNew,maxCombinationsPerCombination,directionToOutput,stack,
											doCheckThis,doCheckInputs,doCheckOutputs,traverseInputs))
										{
											debugscRem();
											if(debugMode) std::cout<<debugsc<<"Matches returned true"<<std::endl;
											
											outputPointersCombination=outputPointersCombinationNew;
										}
										else
										{
											debugscRem();
											if(debugMode) std::cout<<debugsc<<"Matches returned false"<<std::endl;
											
											combinationWorks=false;
											break;
										}
									}
									if(combinationWorks)
									{
										outputPointers=outputPointersCombination;
										combinationFound=true;
										break;
									}
								}
								return combinationFound;
							}
							bool checkInputs(ComputerData& computer,ComputerOutputData& computerOutputData,const ComputerData::Pointer& componentInComputer,
								const ComputerData::Pointer& componentInPattern,
								const ComponentPointers& inputPointers,ComponentPointers& outputPointers,int maxCombinations,bool directionToOutput,
								ComponentPointerStack& stack,bool doCheckThis,bool doCheckInputs,bool doCheckOutputs,bool traverseInputs)
							{
								StackFrame stackFrame(stack);
								
								outputPointers=inputPointers;
								
								ComponentPointers outputPointersNew(stack);
								
								if(debugMode) std::cout<<debugsc<<"Check inputs:"<<std::endl;
								
								if(!(componentInPattern.type==ComputerData::Pointer::Type::memory && !directionToOutput)
									&& componentInPattern.type!=ComputerData::Pointer::Type::input)
								{
									vector<ComputerData::Pointer> patternInputs=getInputsCanRepeat(pattern,componentInPattern);
									int patternInputCount=patternInputs.size();
									vector<ComputerData::Pointer> patternInputsRemoved=
										removeFoundPatternComponents(patternInputs,outputPointers,ComputerData::Pointer());
									
									vector<ComputerData::Pointer> computerInputs=getInputsCanRepeat(computer,componentInComputer);
									int computerInputCount=computerInputs.size();
									vector<ComputerData::Pointer> computerInputsRemoved=removeFoundComputerComponents(computerInputs,outputPointers);
									
									if(computerInputCount!=patternInputCount) return false;
									
									if(!removedFoundComponentsAreTheSame(patternInputsRemoved,computerInputsRemoved,outputPointers))
									{
										return false;
									}
									
									if(patternInputs.size()>0)
									{
										if(computerInputs.size()>0)
										{
											if(computerInputs.size()<patternInputs.size()) return false;
											
											if(matchesWithCombinations(computer,computerOutputData,
												computerInputs,
												patternInputs,
												outputPointers,outputPointersNew,maxCombinations,false,stack,
												doCheckThis,doCheckInputs,doCheckOutputs,traverseInputs))
											{
												outputPointers=outputPointersNew;
											}
											else
											{
												return false;
											}
										}
										else return false;
									}
								}
								
								return true;
							}
							bool checkOutputs(ComputerData& computer,ComputerOutputData& computerOutputData,const ComputerData::Pointer& componentInComputer,
								const ComputerData::Pointer& componentInPattern,
								const ComponentPointers& inputPointers,ComponentPointers& outputPointers,int maxCombinations,bool directionToOutput,
								const ComputerData::Pointer& outputOfComponentActingAsOutput,
								ComponentPointerStack& stack,bool doCheckThis,bool doCheckInputs,bool doCheckOutputs,bool traverseInputs)
							{
								StackFrame stackFrame(stack);
								
								outputPointers=inputPointers;
								
								ComponentPointers outputPointersNew(stack);
								
								if(debugMode) std::cout<<debugsc<<"Check outputs:"<<std::endl;
								
								if(!(componentInPattern.type==ComputerData::Pointer::Type::memory && directionToOutput)
									&& componentInPattern.type!=ComputerData::Pointer::Type::output
									&& componentInComputer.type!=ComputerData::Pointer::Type::constant)
								{
									bool dontCareForExtraOutputsInComputer=
										!outputOfComponentActingAsOutput.isNone() || componentInPattern.type==ComputerData::Pointer::Type::input;
									
									vector<ComputerData::Pointer> patternOutputs=getOutputs(patternOutputData,componentInPattern);
									int patternOutputCount=patternOutputs.size();
									vector<ComputerData::Pointer> patternOutputsRemoved=
										removeFoundPatternComponents(patternOutputs,outputPointers,outputOfComponentActingAsOutput);
									
									vector<ComputerData::Pointer> computerOutputs=getOutputs(computerOutputData,componentInComputer);
									int computerOutputCount=computerOutputs.size();
									vector<ComputerData::Pointer> computerOutputsRemoved=removeFoundComputerComponents(computerOutputs,outputPointers);
									
									if(dontCareForExtraOutputsInComputer){}
									else
									{
										if(computerOutputCount!=patternOutputCount) return false;
									}
									
									if(!removedFoundComponentsAreTheSame(patternOutputsRemoved,computerOutputsRemoved,outputPointers))
									{
										return false;
									}
									
									if(patternOutputs.size()>0)
									{
										if(computerOutputs.size()>0)
										{
											if(computerOutputs.size()<patternOutputs.size()) return false;
											
											if(matchesWithCombinations(computer,computerOutputData,
												computerOutputs,
												patternOutputs,
												outputPointers,outputPointersNew,maxCombinations,true,stack,
												doCheckThis,doCheckInputs,doCheckOutputs,traverseInputs))
											{
												outputPointers=outputPointersNew;
											}
											else
											{
												return false;
											}
										}
										else return false;
									}
								}
								
								return true;
							}
							bool matchesWith(ComputerData& computer,ComputerOutputData& computerOutputData,const ComputerData::Pointer& componentInComputer,
								const ComputerData::Pointer& componentInPattern,
								const ComponentPointers& inputPointers,ComponentPointers& outputPointers,int maxCombinations,bool directionToOutput,
								ComponentPointerStack& stack,bool doCheckThis,bool doCheckInputs,bool doCheckOutputs,bool traverseInputs)
							{
								StackFrame stackFrame(stack);
								
								outputPointers=inputPointers;
								
								matchesWith_count++;
								
								if(doCheckThis)
								{
									if(debugMode) std::cout<<debugsc<<"Check component "<<(directionToOutput?"out":"in")<<" "<<componentInPattern.toString()<<" in pattern. "
										<<componentInComputer.toString()<<" in computer"<<std::endl;
									
									if(componentInPattern.type==ComputerData::Pointer::Type::constant)
									{
										return componentInComputer==componentInPattern;
									}
									else if(componentInPattern.type==ComputerData::Pointer::Type::input)
									{
										if(componentInComputer.type==ComputerData::Pointer::Type::output) return false;
										if(componentInComputer.type==ComputerData::Pointer::Type::constant)
										{
											if(computerOutputData.getOutputsPtr(componentInPattern)->size()!=1) return false;
										}
									}
									else if(componentInComputer.type!=componentInPattern.type) return false;
									
									if(isComponentAlreadyFoundAndIsItTheSame(componentInPattern,componentInComputer,outputPointers))
									{
										return true;
									}
									if(!addFoundComponent(componentInPattern,componentInComputer,outputPointers))
									{
										return false;
									}
								}
								
								ComponentPointers outputPointersNew(stack);
								
								if(doCheckInputs)
								{
									if(checkInputs(computer,computerOutputData,componentInComputer,componentInPattern,
										outputPointers,outputPointersNew,maxCombinations,directionToOutput,
										stack,true,true,false,false))
									{
										outputPointers=outputPointersNew;
									}
									else return false;
								}
								
								if(traverseInputs && !(componentInPattern.type==ComputerData::Pointer::Type::memory && !directionToOutput))
								{
									if(debugMode) std::cout<<debugsc<<"Traverse inputs"<<std::endl;
									
									vector<ComputerData::Pointer> inputs=componentInPattern.getInputs(pattern);
									for(int i=0;i<inputs.size();i++)
									{
										ComputerData::Pointer inputInComputer=outputPointers.translate(inputs[i]);
										if(inputInComputer.isNone())
										{
											throw string()+"At traverse inputs: input in computer is not found";
										}
										
										debugscPut();
										
										if(matchesWith(computer,computerOutputData,inputInComputer,
											inputs[i],
											outputPointers,outputPointersNew,maxCombinations,false,
											stack,false,false,true,true))
										{
											debugscRem();
											
											outputPointers=outputPointersNew;
										}
										else
										{
											debugscRem();
											
											return false;
										}
									}
								}
								
								if(doCheckOutputs)
								{
									ComputerData::Pointer availablePatternOutput;
									if(componentInPattern.type==ComputerData::Pointer::Type::nandGate
										|| componentInPattern.type==ComputerData::Pointer::Type::memory)
									{
										vector<ComputerData::Pointer>*patternOutputsPtr=getOutputsPtr(patternOutputData,componentInPattern);
										if(patternOutputsPtr!=nullptr)
										{
											for(int i=0;i<patternOutputsPtr->size();i++)
											{
												ComputerData::Pointer& patternOutput=(*patternOutputsPtr)[i];
												if(patternOutput.type==ComputerData::Pointer::Type::output)
												{
													if(!outputPointers.isAlreadyFound(patternOutput))
													{
														availablePatternOutput=patternOutput;
														break;
													}
												}
											}
											
											if(!availablePatternOutput.isNone())
											{
												if(!outputPointers.bindToOutput(componentInPattern,availablePatternOutput)) return false;
											}
										}
									}
									
									if(checkOutputs(computer,computerOutputData,componentInComputer,componentInPattern,
										outputPointers,outputPointersNew,maxCombinations,directionToOutput,availablePatternOutput,
										stack,true,true,true,true))
									{
										outputPointers=outputPointersNew;
									}
									else return false;
								}
								
								return true;
							}
							ComputerData::Pointer chooseMainOutput(ComputerData& computer,ComputerOutputData& computerOutputData)
							{
								if(computer.nandGates.size()>0)
								{
									return ComputerData::Pointer(ComputerData::Pointer::Type::nandGate,computer.nandGates.size()-1);
								}
								else if(computer.memory.size()>0)
								{
									return ComputerData::Pointer(ComputerData::Pointer::Type::memory,computer.memory.size()-1);
								}
								else
								{
									return ComputerData::Pointer(ComputerData::Pointer::Type::output,computer.outputs.size()-1);
								}
							}
							MessageQueue*messageQueuePtr=nullptr;
							void addToMessageQueue(const string& messageType,bool isWarning,const string& message)
							{
								if(messageQueuePtr!=nullptr)
								{
									messageQueuePtr->add(messageType,isWarning,message);
								}
							}
							void setMessageQueue(MessageQueue& messageQueue)
							{
								messageQueuePtr=&messageQueue;
							}
							static constexpr bool debugMode=false;
							string debugsc;
							inline void debugscPut()
							{
								if(debugMode)
								{
									if(debugsc.size()<32) debugsc+=" ";
								}
							}
							inline void debugscRem()
							{
								if(debugMode)
								{
									if(debugsc.size()>0) debugsc.resize(debugsc.size()-1);
								}
							}
						public:
						
						uint64_t matchesWith_count=0;
						
						private:
							bool matches(ComputerData& computer,ComputerOutputData& computerOutputData,const ComputerData::Pointer& mainOutputComponent,
								ComponentPointers& outputPointers,int maxCombinations,ComponentPointerStack& stack)
							{
								StackFrame stackFrame(stack);
								
								ComponentPointers inputPointers(stack);
								
								if(debugMode) std::cout<<debugsc<<"\nmainOutputComponent: "<<mainOutputComponent.toString()<<"\n"<<std::endl;
								debugscPut();
								
								if(matchesWith(computer,computerOutputData,
									mainOutputComponent,patternMainOutput,inputPointers,outputPointers,maxCombinations,
									true,stack,true,true,true,true
									))
								{
									debugscRem();
									if(debugMode) std::cout<<debugsc<<"Matches returned true"<<std::endl;
									if(!outputPointers.foundAll())
									{
										throw string()+"The pattern is not a single connected network";
									}
									return true;
								}
								else
								{
									debugscRem();
									if(debugMode) std::cout<<debugsc<<"Matches returned false"<<std::endl;
									return false;
								}
							}
							void applyConnectionChange(ComputerData& computer,ComputerOutputData& computerOutputData,const ConnectionChange& change)
							{
								vector<ComputerData::Pointer*> inputPtrs=change.outputComponent.getInputPtrs(computer);
								if(change.outputComponentInputIndex<0 || change.outputComponentInputIndex>=inputPtrs.size())
								{
									throw string()+"Trying to apply connection change with output component input index out of range";
								}
								*inputPtrs[change.outputComponentInputIndex]=change.newInputComponent;
								
								computerOutputData.change(change);
							}
							void addConnectionChangesToSubstituteInputsToResult(ComputerData& computer,ComponentPointers& pointers,
								const ComputerData::Pointer& pointerInPattern,
								const ComputerData::Pointer& pointerInComputer,
								vector<ConnectionChange>& connectionChanges)
							{
								vector<ComputerData::Pointer> inputs=pointerInComputer.getInputs(computer);
								vector<ComputerData::Pointer> inputPointersInResultingPattern=pointerInPattern.getInputs(resultingPattern);
								if(inputs.size()!=inputPointersInResultingPattern.size()) throw string()+"Input counts do not match";
								for(int i=0;i<inputs.size();i++)
								{
									connectionChanges.emplace_back(
										inputs[i],pointers.translate(inputPointersInResultingPattern[i]),
										i,pointerInComputer
										);
								}
							}
							void execute(ComputerData& computer,ComputerOutputData& computerOutputData,ComponentPointers& pointers,vector<int>& optimizedOutNandGates)
							{
								vector<ConnectionChange> connectionChanges;
								
								vector<int> changesSteps;
								
								vector<ComputerData::Pointer> outputPointersPatternToComputer=pointers.getOutputs();
								vector<ComputerData::Pointer> memoryPointersPatternToComputer=pointers.getMemory();
								vector<ComputerData::Pointer> nandGatePointersPatternToComputer=pointers.getNandGates();
								
								vector<ComputerData::Pointer> outputsRedirectedInPattern=pointers.getOutputsRedirectedInPattern();
								
								for(int i=0;i<outputPointersPatternToComputer.size();i++)
								{
									ComputerData::Pointer pointerInPattern(ComputerData::Pointer::Type::output,i);
									ComputerData::Pointer pointerInComputer=outputPointersPatternToComputer[i];
									
									if(pointerInComputer.isNone()) continue;
									
									addConnectionChangesToSubstituteInputsToResult(computer,pointers,pointerInPattern,pointerInComputer,connectionChanges);
								}
								
								if(debugMode) changesSteps.push_back(connectionChanges.size());
								
								for(int i=0;i<memoryPointersPatternToComputer.size();i++)
								{
									ComputerData::Pointer pointerInPattern(ComputerData::Pointer::Type::memory,i);
									ComputerData::Pointer pointerInComputer=memoryPointersPatternToComputer[i];
									
									addConnectionChangesToSubstituteInputsToResult(computer,pointers,pointerInPattern,pointerInComputer,connectionChanges);
								}
								
								if(debugMode) changesSteps.push_back(connectionChanges.size());
								
								for(int i=0;i<nandGatePointersPatternToComputer.size();i++)
								{
									ComputerData::Pointer pointerInPattern(ComputerData::Pointer::Type::nandGate,i);
									ComputerData::Pointer pointerInComputer=nandGatePointersPatternToComputer[i];
									
									if(i<resultingPattern.nandGates.size())
									{
										addConnectionChangesToSubstituteInputsToResult(computer,pointers,pointerInPattern,pointerInComputer,connectionChanges);
									}
									else
									{
										optimizedOutNandGates[pointerInComputer.index]=true;
										
										vector<ComputerData::Pointer> inputs=pointerInComputer.getInputs(computer);
										for(int j=0;j<inputs.size();j++)
										{
											connectionChanges.emplace_back(
												inputs[j],ComputerData::Pointer(ComputerData::Pointer::Type::constant,0),
												j,pointerInComputer
												);
										}
									}
								}
								
								if(debugMode) changesSteps.push_back(connectionChanges.size());
								
								for(int i=0;i<outputsRedirectedInPattern.size();i++)
								{
									ComputerData::Pointer outputPointerInPattern(ComputerData::Pointer::Type::output,i);
									
									ComputerData::Pointer pointerInPattern=outputsRedirectedInPattern[i];
									if(pointerInPattern.isNone()) continue;
									
									ComputerData::Pointer pointerInComputer=pointers.translate(pointerInPattern);
									
									vector<ComputerData::Pointer> outputs=computerOutputData.getOutputs(pointerInComputer);
									
									ComputerData::Pointer outputInputPointerInPattern=outputPointerInPattern.getInputs(resultingPattern)[0];
									
									ComputerData::Pointer pointerInComputerRedirected=pointers.translate(outputInputPointerInPattern);
									
									for(int o=0;o<outputs.size();o++)
									{
										vector<ComputerData::Pointer> inputs=outputs[o].getInputs(computer);
										
										for(int j=0;j<inputs.size();j++)
										{
											if(inputs[j]==pointerInComputer)
											{
												connectionChanges.emplace_back(
													inputs[j],pointerInComputerRedirected,
													j,outputs[o]
													);
											}
										}
									}
								}
								
								if(debugMode)
								{
									std::cout<<"\n\n\n"<<std::endl;
									for(int i=0;i<connectionChanges.size();i++)
									{
										for(int j=0;j<changesSteps.size();j++)
										{
											if(changesSteps[j]==i)
											{
												std::cout<<std::endl;
											}
										}
										
										std::cout<<"    "<<connectionChanges[i].outputComponent.toString()
											<<"["<<connectionChanges[i].outputComponentInputIndex<<"] : "<<connectionChanges[i].oldInputComponent.toString()<<"->"
											<<connectionChanges[i].newInputComponent.toString()<<std::endl;
									}
								}
								
								for(int i=0;i<connectionChanges.size();i++)
								{
									applyConnectionChange(computer,computerOutputData,connectionChanges[i]);
								}
							}
						public:
						
						bool executeIfItMatches(ComputerData& computer,ComputerOutputData& computerOutputData,const ComputerData::Pointer& mainOutputComponent,
							vector<int>& optimizedOutNandGates,int maxCombinations,MessageQueue& messageQueue)
						{
							if(mainOutputComponent.type==ComputerData::Pointer::Type::nandGate && optimizedOutNandGates[mainOutputComponent.index])
							{
								return false;
							}
							
							setMessageQueue(messageQueue);
							
							size_t stackSize=8*(pattern.numberOfInputs+pattern.outputs.size()+pattern.memory.size()+pattern.nandGates.size());
							ComponentPointerStack stack(pattern,stackSize);
							ComponentPointers pointers(stack);
							if(matches(computer,computerOutputData,mainOutputComponent,pointers,maxCombinations,stack))
							{
								execute(computer,computerOutputData,pointers,optimizedOutNandGates);
								return true;
							}
							else return false;
						}
						bool optimizesMemory()
						{
							return pattern.memory.size()>0;
						}
					};
					
					vector<Rule> rules;
					
					void addRule(const string& name,const ComputerData& pattern,const ComputerData& resultingPattern)
					{
						if(findWithName(rules,name)!=-1)
						{
							throw string()+"Rule with name '"+name+"' already defined";
						}
						rules.emplace_back(name,pattern,resultingPattern);
					}
					
					static constexpr string ruleNamePrefix="RULE_";
					static constexpr string ruleResultNamePrefix="RULEO_";
					
					void addRules(const string& ruleCode)
					{
						try
						{
							Code code(ruleCode);
							code.compileToComponents();
							
							for(int i=0;i<code.components.size();i++)
							{
								string name=code.components[i].name;
								if(name.size()>ruleNamePrefix.size() && name.substr(0,ruleNamePrefix.size())==ruleNamePrefix)
								{
									string justTheName=name.substr(ruleNamePrefix.size(),name.size()-ruleNamePrefix.size());
									string ruleOutputName=ruleResultNamePrefix+justTheName;
									
									try
									{
										ComputerData pattern;
										code.compileComponents(pattern,name);
										
										ComputerData resultingPattern;
										code.compileComponents(resultingPattern,ruleOutputName);
										
										addRule(justTheName,pattern,resultingPattern);
									}
									catch(const string& str)
									{
										throw string()+"at rule '"+justTheName+"' ("+name+"): "+str;
									}
								}
							}
						}
						catch(const string& str)
						{
							throw string()+"Error in optimization rule code: "+str;
						}
					}
				public:
				
				Optimizer(const string& optimizationRulesCodeText=string())
				{
					if(optimizationRulesCodeText.size()>0)
					{
						addRules(optimizationRulesCodeText);
					}
				}
				void optimize(ComputerData& computer,const OptimizationOptions& options)
				{
					if(!options.optimizeGates && !options.optimizeMemory) return;
					
					TimeCounter optimizationTimeCounter;
					optimizationTimeCounter.start();
					
					if(options.optimizeGates)
					{
						int prunedCount=pruneUnusedNandGates(computer);
						if(!options.silent) std::cout<<"Gates removed: "<<prunedCount<<std::endl;
					}
					if(options.optimizeMemory)
					{
						int prunedCount=pruneUnusedMemory(computer);
						if(!options.silent) std::cout<<"Memory removed: "<<prunedCount<<std::endl;
					}
					
					MessageQueue messageQueue("At optimization (delayed message): ",100);
					if(options.silent)
					{
						messageQueue.disableOutput();
					}
					if(options.failAtWarning)
					{
						messageQueue.setThrowAtWarning(true);
					}
					
					ComputerOutputData computerOutputData;
					
					if(options.optimizeGates && rules.size()>0 || options.optimizeMemory)
					{
						for(int passIndex=0;passIndex<options.passes || options.passes==-1;passIndex++)
						{
							if(!options.silent) std::cout<<"\n"<<"Optimization pass "<<(passIndex+1)<<std::endl;
							
							int changeCount=0;
							
							computerOutputData.update(computer);
							
							TimeCounter showProgressTimeCounter;
							showProgressTimeCounter.start();
							
							vector<ComputerData::Pointer> pointers;
							
							for(int i=0;i<computer.nandGates.size();i++)
							{
								pointers.emplace_back(ComputerData::Pointer::Type::nandGate,i);
							}
							for(int i=0;i<computer.memory.size();i++)
							{
								pointers.emplace_back(ComputerData::Pointer::Type::memory,i);
							}
							for(int i=0;i<computer.outputs.size();i++)
							{
								pointers.emplace_back(ComputerData::Pointer::Type::output,i);
							}
							
							messageQueue.setMessagesPerFlush(pointers.size()/10+10);
							
							if(options.optimizeGates && rules.size()>0)
							{
								vector<int> optimizedOutNandGates(computer.nandGates.size(),false);
								int optimizationCount=0;
								vector<int> ruleOptimizationCounts(rules.size(),0);
								for(int i=0;i<pointers.size();i++)
								{
									if(options.maxTime!=-1 && optimizationTimeCounter.end()>=options.maxTime)
									{
										if(!options.silent) std::cout<<"Stopping ("<<optimizationTimeCounter.end()<<" s >= "<<options.maxTime<<" s"<<")"<<std::endl;
										break;
									}
									
									if(i%(pointers.size()/10+1)==0 || showProgressTimeCounter.end()>=10)
									{
										if(!options.silent) std::cout<<pointers[i].toString()<<" ("<<(i*100/pointers.size())<<"%)"<<std::endl;
										showProgressTimeCounter.start();
									}
									
									for(int j=0;j<rules.size();j++)
									{
										if(options.maxTime!=-1 && optimizationTimeCounter.end()>=options.maxTime) break;
										
										if(!options.optimizeMemory && rules[j].optimizesMemory()) continue;
										
										if(rules[j].executeIfItMatches(computer,computerOutputData,pointers[i],
											optimizedOutNandGates,options.maxCombinations,messageQueue))
										{
											ruleOptimizationCounts[j]++;
											optimizationCount++;
											break;
										}
									}
								}
								changeCount+=optimizationCount;
								
								messageQueue.flush();
								
								if(!options.silent)
								{
									if(options.verbose)
									{
										std::cout<<"\n";
										for(int j=0;j<rules.size();j++)
										{
											if(!options.optimizeMemory && rules[j].optimizesMemory()) continue;
											
											std::cout<<"Rule '"<<rules[j].name<<"' optimizations: "<<ruleOptimizationCounts[j]<<"\n";
										}
										std::cout<<"\n";
										for(int j=0;j<rules.size();j++)
										{
											if(!options.optimizeMemory && rules[j].optimizesMemory()) continue;
											
											std::cout<<"Rule '"<<rules[j].name<<"' total node match tests: "<<rules[j].matchesWith_count<<"\n";
										}
										std::cout<<std::endl;
									}
									
									std::cout<<"Optimizations done: "<<optimizationCount<<std::endl;
								}
							}
							
							if(options.optimizeGates)
							{
								int prunedCount=pruneUnusedNandGates(computer);
								if(!options.silent) std::cout<<"Gates removed: "<<prunedCount<<std::endl;
								changeCount+=prunedCount;
							}
							if(options.optimizeMemory)
							{
								int prunedCount=pruneUnusedMemory(computer);
								if(!options.silent) std::cout<<"Memory removed: "<<prunedCount<<std::endl;
								changeCount+=prunedCount;
							}
							
							if(!options.silent) std::cout<<"Total elapsed time: "<<optimizationTimeCounter.end()<<" s"<<std::endl;
							
							if(changeCount==0) break;
							if(options.maxTime!=-1 && optimizationTimeCounter.end()>=options.maxTime) break;
						}
					}
				}
			};
			
			void optimize(Optimizer& optimizer,ComputerData& computer,const OptimizationOptions& optimizationOptions)
			{
				try
				{
					optimizer.optimize(computer,optimizationOptions);
					
					reorderNandGatesInValidOrder(computer);
				}
				catch(const string& str)
				{
					throw string()+"Internal error during optimization: "+str;
				}
			}
			void compileToComponents()
			{
				processTemplates();
				
				readComponents();
				
				processComponents();
				
				checkRecursivity();
			}
		public:
		ComputerData compileToComputerData()
		{
			compileToComponents();
			
			ComputerData computer;
			compileComponents(computer);
			
			return computer;
		}
		Computer compile(const OptimizationOptions& optimizationOptions,const string& optimizationRulesCodeText)
		{
			ComputerData computer=compileToComputerData();
			
			Optimizer optimizer(optimizationRulesCodeText);
			optimize(optimizer,computer,optimizationOptions);
			
			return computer.getComputer();
		}
		
		private:
			static string decompile_getInputName(int index)
			{
				return string()+"i"+std::to_string(index);
			}
			static string decompile_getMemoryName(int index)
			{
				return string()+"m"+std::to_string(index);
			}
			static string decompile_getNandGateName(int index)
			{
				return string()+"n"+std::to_string(index);
			}
			static string decompile_getOutputName(int index)
			{
				return string()+"o"+std::to_string(index);
			}
			static string decompile_getName(const ComputerData::Pointer& pointer)
			{
				if(pointer.type==ComputerData::Pointer::Type::input)
				{
					return decompile_getInputName(pointer.index);
				}
				else if(pointer.type==ComputerData::Pointer::Type::memory)
				{
					return decompile_getMemoryName(pointer.index);
				}
				else if(pointer.type==ComputerData::Pointer::Type::output)
				{
					return decompile_getOutputName(pointer.index);
				}
				else if(pointer.type==ComputerData::Pointer::Type::nandGate)
				{
					return decompile_getNandGateName(pointer.index);
				}
				else if(pointer.type==ComputerData::Pointer::Type::constant)
				{
					return std::to_string(pointer.index);
				}
				else throw string()+"Invalid pointer type";
			}
		public:
		
		static string decompile(const ComputerData& computer,const string& mainComponentName="main")
		{
			string codeString;
			
			try
			{
				codeString+=mainComponentName+":\n\n";
				
				for(int i=0;i<computer.outputs.size();i++)
				{
					codeString+=decompile_getOutputName(i)+": out\n";
				}
				codeString+="\n";
				
				for(int i=0;i<computer.numberOfInputs;i++)
				{
					codeString+=decompile_getInputName(i)+": in\n";
				}
				codeString+="\n";
				
				for(int i=0;i<computer.memory.size();i++)
				{
					codeString+=decompile_getMemoryName(i)+": reg\n";
				}
				codeString+="\n";
				
				for(int i=0;i<computer.nandGates.size();i++)
				{
					codeString+=decompile_getNandGateName(i)+"=nand "
						+decompile_getName(computer.nandGates[i].inputA)
						+" "+decompile_getName(computer.nandGates[i].inputB)
						+"\n";
				}
				codeString+="\n";
				
				for(int i=0;i<computer.memory.size();i++)
				{
					codeString+=decompile_getMemoryName(i)+"=set "+decompile_getName(computer.memory[i].input)+"\n";
				}
				codeString+="\n";
				
				for(int i=0;i<computer.outputs.size();i++)
				{
					codeString+=decompile_getOutputName(i)+"=set "+decompile_getName(computer.outputs[i].input)+"\n";
				}
				codeString+="\n";
			}
			catch(const string& str)
			{
				throw string()+"Error while decompiling: "+str;
			}
			
			return codeString;
		}
	};
	
	private:
		inline static ComputerData::Pointer getNandGateInputPointerFromIndex(int index,int numberOfInputs)
		{
			if(index<numberOfInputs) return ComputerData::Pointer(ComputerData::Pointer::Type::input,index);
			else return ComputerData::Pointer(ComputerData::Pointer::Type::nandGate,index-numberOfInputs);
		}
		inline static int getIndexFromNandGateInputPointer(const ComputerData::Pointer& pointer,int numberOfInputs)
		{
			if(pointer.type==ComputerData::Pointer::Type::input) return pointer.index;
			else return pointer.index+numberOfInputs;
		}
		static void setNandGateInputs(ComputerData& circuit,const vector<vector<int>>& inputs)
		{
			for(int i=0;i<circuit.nandGates.size();i++)
			{
				circuit.nandGates[i].inputA=getNandGateInputPointerFromIndex(inputs[0][i],circuit.numberOfInputs);
				circuit.nandGates[i].inputB=getNandGateInputPointerFromIndex(inputs[1][i],circuit.numberOfInputs);
			}
		}
		static vector<vector<int>> getNandGateInputs(ComputerData& circuit)
		{
			vector<vector<int>> inputs(2,vector<int>(circuit.nandGates.size()));
			for(int i=0;i<circuit.nandGates.size();i++)
			{
				inputs[0][i]=getIndexFromNandGateInputPointer(circuit.nandGates[i].inputA,circuit.numberOfInputs);
				inputs[1][i]=getIndexFromNandGateInputPointer(circuit.nandGates[i].inputB,circuit.numberOfInputs);
			}
			return inputs;
		}
		static int getEqualOrGreaterValidInputB(const vector<vector<int>>& inputs,int numberOfInputs,int gate,int inputA,int inputB)
		{
			int i=inputA;
			for(int j=inputB;j<numberOfInputs+gate;j++)
			{
				bool found=false;
				for(int gb=0;gb<gate;gb++)
				{
					if(inputs[0][gb]==i && inputs[1][gb]==j)
					{
						found=true;
						break;
					}
				}
				if(found) continue;
				if(i==j && i>=numberOfInputs)
				{
					if(inputs[0][i-numberOfInputs]==inputs[1][i-numberOfInputs]) continue;
				}
				return j;
			}
			return -1;
		}
		static bool moveInputsToEqualOrGreaterValidInputs(vector<vector<int>>& inputs,int numberOfInputs,int gate,int inputA,int inputB)
		{
			for(int i=inputA,j=inputB;i<numberOfInputs+gate;i++)
			{
				j=getEqualOrGreaterValidInputB(inputs,numberOfInputs,gate,i,j);
				if(j!=-1)
				{
					inputs[0][gate]=i;
					inputs[1][gate]=j;
					return true;
				}
				else j=i+1;
			}
			return false;
		}
		static bool moveInputsToFirstValidCombination(vector<vector<int>>& inputs,int numberOfInputs,int numberOfGates,int startGate)
		{
			for(int gate=startGate;gate<numberOfGates;gate++)
			{
				if(!moveInputsToEqualOrGreaterValidInputs(inputs,numberOfInputs,gate,0,0))
				{
					return false;
				}
			}
			return true;
		}
		static bool moveInputsToNextValidCombination(vector<vector<int>>& inputs,int numberOfInputs,int numberOfGates)
		{
			for(int startGate=numberOfGates-1;startGate>=0;startGate--)
			{
				if(moveInputsToEqualOrGreaterValidInputs(inputs,numberOfInputs,startGate,inputs[0][startGate],inputs[1][startGate]+1))
				{
					if(startGate==numberOfGates-1) return true;
					else if(moveInputsToFirstValidCombination(inputs,numberOfInputs,numberOfGates,startGate+1))
					{
						return true;
					}
				}
			}
			return false;
		}
		static ComputerData getFirstCircuitCombination(int numberOfInputs,int numberOfGates,bool& success)
		{
			ComputerData circuit;
			
			circuit.addInputs(numberOfInputs);
			for(int i=0;i<numberOfInputs;i++)
			{
				circuit.addOutput(ComputerData::Pointer(ComputerData::Pointer::Type::input,i));
			}
			
			ComputerData::Pointer pointer(ComputerData::Pointer::Type::input,0);
			for(int i=0;i<numberOfGates;i++)
			{
				circuit.addNandGate(ComputerData::NandGate(pointer,pointer));
				
				circuit.addOutput(ComputerData::Pointer(ComputerData::Pointer::Type::nandGate,i));
			}
			
			if(numberOfGates==0)
			{
				success=true;
				
				return circuit;
			}
			
			vector<vector<int>> inputs=getNandGateInputs(circuit);
			
			if(!moveInputsToFirstValidCombination(inputs,numberOfInputs,numberOfGates,0))
			{
				success=false;
				
				return circuit;
			}
			else
			{
				success=true;
				
				setNandGateInputs(circuit,inputs);
				
				return circuit;
			}
		}
		static bool incrementCircuitCombination(ComputerData& circuit)
		{
			vector<vector<int>> inputs=getNandGateInputs(circuit);
			
			int numberOfInputs=circuit.numberOfInputs;
			int numberOfGates=circuit.nandGates.size();
			
			if(numberOfGates==0) return false;
			
			if(!moveInputsToNextValidCombination(inputs,numberOfInputs,numberOfGates)) return false;
			
			setNandGateInputs(circuit,inputs);
			
			return true;
		}
		static bool circuitOutputsMatch(const ComputerData& referenceCircuit,const ComputerData& circuit,ComputerData& circuitWithCorrectOutputs)
		{
			int maxInputs=32;
			if(referenceCircuit.numberOfInputs>maxInputs)
			{
				throw string()+"The circuit has too many inputs ("
					+std::to_string(referenceCircuit.numberOfInputs)+">"+std::to_string(maxInputs)+")";
			}
			
			uint64_t inputCombinations=(uint64_t(1)<<referenceCircuit.numberOfInputs);
			
			Computer referenceComputer=referenceCircuit.getComputer();
			Computer computer=circuit.getComputer();
			
			vector<vector<int>> outputCandidates(referenceCircuit.outputs.size(),vector<int>(circuit.outputs.size(),true));
			vector<int> outputCandidateCounts(referenceCircuit.outputs.size(),circuit.outputs.size());
			
			constexpr uint64_t combinationsPerBatch=64;
			
			using State=Computer::State<uint64_t>;
			
			State referenceInitialState=referenceComputer.getInitialState<State>(combinationsPerBatch);
			State initialState=computer.getInitialState<State>(combinationsPerBatch);
			State referenceState;
			State state;
			
			for(uint64_t batch=0;batch*combinationsPerBatch<inputCombinations;batch++)
			{
				uint64_t firstCombination=batch*combinationsPerBatch;
				
				uint64_t combinationsThisBatch=inputCombinations-firstCombination;
				if(combinationsThisBatch>combinationsPerBatch) combinationsThisBatch=combinationsPerBatch;
				
				for(int i=0;i<initialState.inputs.size();i++)
				{
					uint64_t number=0;
					for(int b=0;b<combinationsThisBatch;b++)
					{
						number|=((((firstCombination+b)>>i)&1)<<b);
					}
					
					referenceInitialState.inputs[i]=number;
					initialState.inputs[i]=number;
				}
				
				referenceState=referenceComputer.simulateStep(referenceInitialState);
				state=computer.simulateStep(initialState);
				
				for(int i=0;i<referenceState.outputs.size();i++)
				{
					for(int j=0;j<state.outputs.size();j++)
					{
						if(!outputCandidates[i][j]) continue;
						if(state.outputs[j]!=referenceState.outputs[i])
						{
							outputCandidates[i][j]=false;
							outputCandidateCounts[i]--;
							if(outputCandidateCounts[i]==0)
							{
								return false;
							}
						}
					}
				}
			}
			
			circuitWithCorrectOutputs=circuit;
			circuitWithCorrectOutputs.outputs.resize(0);
			
			for(int i=0;i<outputCandidates.size();i++)
			{
				int index=-1;
				for(int j=0;j<outputCandidates[i].size();j++)
				{
					if(outputCandidates[i][j])
					{
						index=j;
						break;
					}
				}
				if(index==-1) return false;
				circuitWithCorrectOutputs.addOutput(circuit.outputs[index].input);
			}
			
			return true;
		}
		
		static ComputerData optimizationSearch_bruteForce(const ComputerData& referenceCircuit,const OptimizationSearchOptions& options)
		{
			try
			{
				if(referenceCircuit.memory.size()>0)
				{
					throw string()+"There is memory in the input circuit for the brute force optimization search";
				}
				
				ComputerData bestCandidate=referenceCircuit;
				
				if(referenceCircuit.numberOfInputs<=0)
				{
					throw string()+"The circuit has no inputs";
				}
				if(referenceCircuit.outputs.size()==0)
				{
					throw string()+"The circuit has no outputs";
				}
				
				bool foundBetterSolution=false;
				
				bool print=!options.silent;
				double maxTime=options.maxTime;
				
				if(print) std::cout<<"Input circuit with "
					<<referenceCircuit.nandGates.size()<<" gate"<<(referenceCircuit.nandGates.size()==1 ? "" : "s")
					<<std::endl;
				
				int maxGates=options.maxGates;
				if(maxGates<0) maxGates+=referenceCircuit.nandGates.size();
				if(maxGates<0) maxGates=0;
				if(maxGates>=referenceCircuit.nandGates.size())
				{
					if(print) std::cout<<"Returning the input circuit (it has the required number of gates)"<<std::endl;
					return referenceCircuit;
				}
				
				TimeCounter searchTimeCounter;
				searchTimeCounter.start();
				
				for(int numberOfGates=0;numberOfGates<=maxGates;numberOfGates++)
				{
					if(print) std::cout<<"\n"<<"Searching with "<<numberOfGates<<" gate"<<(numberOfGates==1 ? "" : "s")<<"..."<<"\n"<<std::endl;
					
					bool firstCombinationSuccess=true;
					ComputerData circuit=getFirstCircuitCombination(referenceCircuit.numberOfInputs,numberOfGates,firstCombinationSuccess);
					if(!firstCombinationSuccess)
					{
						if(print) std::cout<<"Could not generate the first combination of gates"<<std::endl;
						break;
					}
					
					TimeCounter showProgressTimeCounter;
					showProgressTimeCounter.start();
					
					ComputerData circuitWithCorrectOutputs;
					
					for(uint64_t combination=0;;combination++)
					{
						if(maxTime!=-1 && searchTimeCounter.end()>=maxTime)
						{
							break;
						}
						
						if(showProgressTimeCounter.end()>=10)
						{
							if(print) std::cout<<"Combination "<<combination<<std::endl;
							showProgressTimeCounter.start();
						}
						
						if(circuitOutputsMatch(referenceCircuit,circuit,circuitWithCorrectOutputs))
						{
							foundBetterSolution=true;
							bestCandidate=circuitWithCorrectOutputs;
							
							if(print) std::cout<<"Found circuit with "<<numberOfGates<<" gate"<<(numberOfGates==1 ? "" : "s")<<" ("<<searchTimeCounter.end()<<" s)"<<std::endl;
							
							break;
						}
						
						if(!incrementCircuitCombination(circuit)) break;
					}
					if(foundBetterSolution)
					{
						break;
					}
					if(maxTime!=-1 && searchTimeCounter.end()>=maxTime)
					{
						if(print) std::cout<<"Stopping ("<<searchTimeCounter.end()<<" s >= "<<maxTime<<"s)"<<std::endl;
						break;
					}
				}
				
				if(!foundBetterSolution)
				{
					if(print) std::cout<<"A better circuit with the specified requirements has not been found. Returning the input circuit"<<std::endl;
				}
				
				return bestCandidate;
			}
			catch(const string& str)
			{
				throw string()+"Error during optimization search: "+str;
			}
		}
	public:
	
	static string optimizationSearch_bruteForce(const string& inputCode,const OptimizationSearchOptions& options)
	{
		Code code(inputCode);
		ComputerData referenceCircuit=code.compileToComputerData();
		
		Code::pruneUnusedNandGates(referenceCircuit);
		
		return Code::decompile(optimizationSearch_bruteForce(referenceCircuit,options));
	}
	
	static string optimizationSearch(const string& inputCode,const OptimizationSearchOptions& options)
	{
		if(options.algorithm=="bruteforce") return optimizationSearch_bruteForce(inputCode,options);
		else throw string()+"Algorithm not recognized";
	}
	
	Computer buildComputer(const string& codeText,const OptimizationOptions& optimizationOptions,const string& optimizationRulesCodeText)
	{
		Code code(codeText);
		Computer computer=code.compile(optimizationOptions,optimizationRulesCodeText);
		
		string trace;
		if(!computer.checkValidity(trace))
		{
			throw string("Computer not valid. Showing trace:\n")+trace;
		}
		
		return computer;
	}
};
