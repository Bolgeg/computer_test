class ComputerBuilder
{
	public:
	
	class ComputerData
	{
		public:
		
		class Input
		{
			public:
			
			enum class Type{computerInput,computerMemory,nandGate,constant,yesGate};
			
			Type type=Type::constant;
			int index=0;
			
			Input(){}
			Input(const Type& _type,int _index)
			{
				type=_type;
				index=_index;
			}
		};
		
		class YesGate
		{
			public:
			
			bool inputResolved=false;
			Input input;
			
			void setInput(const Input& _input)
			{
				inputResolved=true;
				input=_input;
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
		};
		
		class NandGate
		{
			public:
			
			Input inputA;
			Input inputB;
			
			int lineIndexesIndex=-1;
			
			NandGate(){}
			NandGate(const Input& _inputA,const Input& _inputB)
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
		int addMemory(const Input& memoryOutputGateInput)
		{
			int index=memory.size();
			memory.emplace_back(memoryOutputGateInput);
			return index;
		}
		void setMemory(int index,const Input& memoryOutputGateInput)
		{
			memory[index]=OutputGate(memoryOutputGateInput);
		}
		int addOutput(const Input& outputOutputGateInput)
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
		void setYesGateInput(int index,const Input& yesGateInput)
		{
			yesGates[index].setInput(yesGateInput);
		}
		
		private:
			Computer::Input getComputerInput(const Input& input)
			{
				Computer::Input::Type type=Computer::Input::Type::computerInput;
				if(input.type==Input::Type::computerInput){}
				else if(input.type==Input::Type::computerMemory) type=Computer::Input::Type::computerMemory;
				else if(input.type==Input::Type::nandGate) type=Computer::Input::Type::nandGate;
				else if(input.type==Input::Type::constant) type=Computer::Input::Type::constant;
				else
				{
					throw string("Error: Invalid gate for final computer");
				}
				
				return Computer::Input(type,input.index);
			}
		public:
		Computer getComputer()
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
	
	class Code
	{
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
			
			vector<Variable> variables;
			
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
		Code(const string& codeText)
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
			
			template <class ValueT,class ContextT>
			class ExpressionEvaluator
			{
				public:
				
				class Code
				{
					public:
					
					vector<string> tokens;
					
					Code(){}
					explicit Code(const string& codeString)
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
				
				class EvaluatorContext
				{
					public:
					
					const Code*codePtr=nullptr;
					size_t tokenIndex=0;
					int subexpressionLevel=0;
					
					EvaluatorContext(){}
					EvaluatorContext(const Code& _code,size_t _tokenIndex,int _subexpressionLevel)
					{
						codePtr=&_code;
						tokenIndex=_tokenIndex;
						subexpressionLevel=_subexpressionLevel;
					}
				};
				
				class Function
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
								if(i>0 && nameTokens[i].size()>1) throw string()+"Internal error: operator with separator or end strings with multiple tokens (not supported)";
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
								nameTokens.push_back(Code::tokenize(nameVector[i]));
							}
							name= nameTokens.size()>0 ? Code::untokenize(nameTokens[0]) : string();
							
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
					
					Function(){}
					
					template <class NameType>
					Function(const NameType& _name,const FunctionType& _function,int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,nullptr,_numberOfArguments,_allowSkippedArguments);
					}
					template <class NameType>
					Function(const NameType& _name,const FunctionTypeNoEvaluatorContext& _function,int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,nullptr,_numberOfArguments,_allowSkippedArguments);
					}
					template <class NameType>
					Function(const NameType& _name,const FunctionTypeNoContext& _function,int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,nullptr,_numberOfArguments,_allowSkippedArguments);
					}
					
					template <class NameType>
					Function(const NameType& _name,const FunctionType& _function,const CheckFunctionType& _checkFunction,
						int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,_checkFunction,_numberOfArguments,_allowSkippedArguments);
					}
					template <class NameType>
					Function(const NameType& _name,const FunctionTypeNoEvaluatorContext& _function,const CheckFunctionType& _checkFunction,
						int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,_checkFunction,_numberOfArguments,_allowSkippedArguments);
					}
					template <class NameType>
					Function(const NameType& _name,const FunctionTypeNoContext& _function,const CheckFunctionType& _checkFunction,
						int _numberOfArguments=-1,bool _allowSkippedArguments=false)
					{
						initialize(_name,_function,_checkFunction,_numberOfArguments,_allowSkippedArguments);
					}
					
					ValueT execute(const vector<ValueT>& args,ContextT& context,const EvaluatorContext& evaluatorContext) const
					{
						if(!isDefined()) throw string()+"Internal error: executing an operator that is not defined";
						if(numberOfArguments!=-1)
						{
							if(args.size()!=numberOfArguments) throw string()+"wrong number of arguments for function: '"+name+"'";
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
					void setFixedNumberOfArguments(vector<Function>& functionsToModify,int numberOfArguments)
					{
						for(int f=0;f<functionsToModify.size();f++)
						{
							functionsToModify[f].setFixedNumberOfArguments(numberOfArguments);
						}
					}
					void setAsStarterAndSeparator(vector<Function>& functionsToModify)
					{
						for(int f=0;f<functionsToModify.size();f++)
						{
							functionsToModify[f].setAsStarterAndSeparator();
						}
					}
					void setAsStarterAndOptionalSeparatorAndTerminator(vector<Function>& functionsToModify)
					{
						for(int f=0;f<functionsToModify.size();f++)
						{
							functionsToModify[f].setAsStarterAndOptionalSeparatorAndTerminator();
						}
					}
					
					vector<Function> unaryOperators;
					vector<Function> binaryOperators;
					vector<vector<string>> binaryOperatorLevels;
					vector<Function> ternaryOperators;
					vector<Function> bracketFunctionOperators;
					vector<Function> bracketOperators;
					
					std::function<ValueT(const string&,ContextT&,const EvaluatorContext&)> tokenResolutionFunction;
					
					bool changed=true;
				public:
				
				void setTokenResolutionFunction(const std::function<ValueT(const string&,ContextT&,const EvaluatorContext&)>& f)
				{
					tokenResolutionFunction=f;
					changed=true;
				}
				void setUnaryOperators(const vector<Function>& f)
				{
					unaryOperators=f;
					setFixedNumberOfArguments(unaryOperators,1);
					changed=true;
				}
				void setBinaryOperators(const vector<Function>& f,const vector<vector<string>>& operatorLevels)
				{
					binaryOperators=f;
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
							string name=Code::untokenize(Code::tokenize(binaryOperatorLevels[i][j]));
							
							Function*op=findFunctionWithName(binaryOperators,name);
							if(op!=nullptr) op->setOperatorLevel(getBinaryOperatorLevel(i));
						}
					}
					for(int i=0;i<binaryOperators.size();i++)
					{
						Function*op=&binaryOperators[i];
						if(op->getOperatorLevel()==-1)
						{
							throw string()+"internal error: binary operator with no defined level: '"+op->getName()+"'";
						}
					}
					
					changed=true;
				}
				void setTernaryOperators(const vector<Function>& f)
				{
					ternaryOperators=f;
					setFixedNumberOfArguments(ternaryOperators,3);
					setAsStarterAndSeparator(ternaryOperators);
					changed=true;
				}
				void setBracketFunctionOperators(const vector<Function>& f)
				{
					bracketFunctionOperators=f;
					setAsStarterAndOptionalSeparatorAndTerminator(bracketFunctionOperators);
					changed=true;
				}
				void setBracketOperators(const vector<Function>& f)
				{
					bracketOperators=f;
					setAsStarterAndOptionalSeparatorAndTerminator(bracketOperators);
					changed=true;
				}
				
				private:
					vector<string> delimiters;
					
					void addDelimiter(const string& str)
					{
						if(str.size()>0 && !isDelimiter(str))
						{
							delimiters.push_back(str);
						}
					}
					void updateChanges()
					{
						delimiters=vector<string>{")",","};
						for(int i=0;i<ternaryOperators.size();i++)
						{
							if(!ternaryOperators[i].isDefined()) continue;
							addDelimiter(getOperatorSeparator(&ternaryOperators[i]));
						}
						for(int i=0;i<bracketFunctionOperators.size();i++)
						{
							if(!bracketFunctionOperators[i].isDefined()) continue;
							addDelimiter(getOperatorSeparator(&bracketFunctionOperators[i]));
							addDelimiter(getOperatorTerminator(&bracketFunctionOperators[i]));
						}
						for(int i=0;i<bracketOperators.size();i++)
						{
							if(!bracketOperators[i].isDefined()) continue;
							addDelimiter(getOperatorSeparator(&bracketOperators[i]));
							addDelimiter(getOperatorTerminator(&bracketOperators[i]));
						}
						
						changed=false;
					}
					
					bool isDelimiter(const string& token)
					{
						for(int i=0;i<delimiters.size();i++)
						{
							if(token==delimiters[i]) return true;
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
					
					bool parseOperator(const Code& code,size_t& tokenIndex,Function*& outputOp,vector<Function>& operators)
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
								
								if(operators[o].getNameTokens()[0].size()<tokens) matches[o]=false;
								else matches[o]= operators[o].getNameTokens()[0][tokens-1]==token;
								
								if(matches[o]) matchIndex=o;
								
								if(matches[o] && operators[o].getNameTokens()[0].size()==tokens)
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
					Function*findFunctionWithName(vector<Function>& f,const string& name)
					{
						for(int i=0;i<f.size();i++)
						{
							if(f[i].getName()==name) return &f[i];
						}
						return nullptr;
					}
					string getOperatorSeparator(Function*op)
					{
						if(op==nullptr) return string();
						vector<string> v=op->getSeparator();
						if(v.size()==0) return string();
						return v[0];
					}
					string getOperatorTerminator(Function*op)
					{
						if(op==nullptr) return string();
						vector<string> v=op->getTerminator();
						if(v.size()==0) return string();
						return v[0];
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
					
					ValueT executeVariableArgumentOperatorEvaluatingArguments(const Code& code,size_t& t,ContextT& context,
						Function*op,const vector<ValueT>& startArguments,int subexpressionLevel)
					{
						vector<ValueT> arguments=startArguments;
						
						string separator=getOperatorSeparator(op);
						string terminator=getOperatorTerminator(op);
						bool allowSkippedArguments=op->getAllowSkippedArguments();
						
						int argumentsWithoutCheck=0;
						
						for(;;)
						{
							if(allowSkippedArguments)
							{
								if(code.tokens[t]==separator || code.tokens[t]==terminator)
								{
									if(tokenResolutionFunction==nullptr) throw unexpectedTokenMessage(code.tokens[t],__LINE__);
									
									if(argumentsWithoutCheck)
									{
										arguments=op->executeCheck(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
										argumentsWithoutCheck=0;
									}
									arguments.push_back(tokenResolutionFunction(string(),context,EvaluatorContext(code,t,subexpressionLevel)));
									argumentsWithoutCheck++;
								}
								else
								{
									if(argumentsWithoutCheck)
									{
										arguments=op->executeCheck(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
										argumentsWithoutCheck=0;
									}
									arguments.push_back(evaluateExpressionPart(code,t,context,0,subexpressionLevel+1));
									argumentsWithoutCheck++;
									
									if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
								}
							}
							else
							{
								if(code.tokens[t]==terminator) break;
								
								if(argumentsWithoutCheck)
								{
									arguments=op->executeCheck(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
									argumentsWithoutCheck=0;
								}
								arguments.push_back(evaluateExpressionPart(code,t,context,0,subexpressionLevel+1));
								argumentsWithoutCheck++;
								
								if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
							}
							
							if(code.tokens[t]==separator)
							{
								t++;
								if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
							}
							else if(code.tokens[t]==terminator) break;
							else throw expectedTokenMessage(terminator,code.tokens[t],__LINE__);
						}
						
						t++;
						
						return op->execute(arguments,context,EvaluatorContext(code,t,subexpressionLevel));
					}
					ValueT evaluateExpressionPart(const Code& code,size_t& t,ContextT& context,int level,int subexpressionLevel)
					{
						if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
						
						string token=code.tokens[t];
						
						ValueT lvalue;
						
						Function*bracketOperator=nullptr;
						if(parseOperator(code,t,bracketOperator,bracketOperators))
						{
							if(t>=code.tokens.size()) throw expectedContinuationMessage(__LINE__);
							
							lvalue=executeVariableArgumentOperatorEvaluatingArguments(code,t,context,bracketOperator,vector<ValueT>(),subexpressionLevel);
						}
						else
						{
							Function*unaryOperator;
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
								lvalue=tokenResolutionFunction(token,context,EvaluatorContext(code,t,subexpressionLevel));
								t++;
							}
						}
						
						for(;;)
						{
							if(t>=code.tokens.size()) break;
							token=code.tokens[t];
							if(isDelimiter(token)) break;
							
							{
								Function*ternaryOperator;
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
										string separator=getOperatorSeparator(ternaryOperator);
										if(code.tokens[t]!=separator) throw expectedTokenMessage(separator,code.tokens[t],__LINE__);
										t++;
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
									Function*bracketFunctionOperator;
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
								Function*binaryOperator;
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
					
					Code code(expressionString);
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
					ExpressionEvaluator<string,Context> expressionEvaluator;
					
					using Function=ExpressionEvaluator<string,Context>::Function;
					using EvaluatorContext=ExpressionEvaluator<string,Context>::EvaluatorContext;
					
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
					
					vector<Function> unaryOperators=vector<Function>
					{
						Function("-",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							
							SafeInteger r=-a;
							
							return safeIntegerToString(r);
						}),
						Function("~",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							
							SafeInteger r=~a;
							
							return safeIntegerToString(r);
						}),
						Function("!",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							
							SafeInteger r=!a;
							
							return safeIntegerToString(r);
						})
					};
					
					vector<Function> binaryOperators=vector<Function>
					{
						Function("**",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r=a.pow(b);
							
							return safeIntegerToString(r);
						}),
						Function("*",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r=a*b;
							
							return safeIntegerToString(r);
						}),
						Function("/",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r=a/b;
							
							return safeIntegerToString(r);
						}),
						Function("%",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r=a%b;
							
							return safeIntegerToString(r);
						}),
						Function("+",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r=a+b;
							
							return safeIntegerToString(r);
						}),
						Function("-",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r=a-b;
							
							return safeIntegerToString(r);
						}),
						Function("<<",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r=(a<<b);
							
							return safeIntegerToString(r);
						}),
						Function(">>",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r=(a>>b);
							
							return safeIntegerToString(r);
						}),
						Function(">>>",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r=a.shiftRightLogical(b);
							
							return safeIntegerToString(r);
						}),
						Function("<",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r= a<b;
							
							return safeIntegerToString(r);
						}),
						Function("<=",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r= a<=b;
							
							return safeIntegerToString(r);
						}),
						Function(">",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r= a>b;
							
							return safeIntegerToString(r);
						}),
						Function(">=",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r= a>=b;
							
							return safeIntegerToString(r);
						}),
						Function("==",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r= a==b;
							
							return safeIntegerToString(r);
						}),
						Function("!=",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r= a!=b;
							
							return safeIntegerToString(r);
						}),
						Function("&",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r= a&b;
							
							return safeIntegerToString(r);
						}),
						Function("^",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r= a^b;
							
							return safeIntegerToString(r);
						}),
						Function("|",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							SafeInteger a=stringToSafeInteger(args[0]);
							SafeInteger b=stringToSafeInteger(args[1]);
							
							SafeInteger r= a|b;
							
							return safeIntegerToString(r);
						}),
						Function("&&",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
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
						Function("||",[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
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
					
					vector<Function> ternaryOperators=vector<Function>{
						Function(vector<string>{"?",":"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
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
					
					vector<Function> bracketOperators=vector<Function>{
						Function(vector<string>{"(",")"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							return args[0];
						},1),
						Function(vector<string>{"isp2(",",",")"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							int a=stringToInt(args[0]);
							int r=a>0 && std::has_single_bit(uint32_t(a));
							
							return intToString(r);
						},1),
						Function(vector<string>{"log2(",",",")"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							int a=stringToInt(args[0]);
							if(a<=0) throw string()+"logarithm of zero or negative number: "+std::to_string(a);
							int r=int(std::countr_zero(std::bit_floor(uint32_t(a))));
							
							return intToString(r);
						},1),
						Function(vector<string>{"p2floor(",",",")"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
						{
							if(!context.updateEvaluationState(evaluatorContext.subexpressionLevel)) return string();
							
							int a=stringToInt(args[0]);
							if(a<=0) throw string()+"invalid zero or negative number for function: "+std::to_string(a);
							int r=int(std::bit_floor(uint32_t(a)));
							
							return intToString(r);
						},1),
						Function(vector<string>{"p2ceil(",",",")"},[this](const vector<string>& args,Context& context,const EvaluatorContext& evaluatorContext)->string
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
						
						vector<Component::Variable> variables;
						
						ParserContext(){}
						explicit ParserContext(const vector<Component::Variable>& _variables)
						{
							variables=_variables;
						}
					};
					
					using ParserFunction=ExpressionEvaluator<InputExpression,ParserContext>::Function;
					using ParserEvaluatorContext=ExpressionEvaluator<InputExpression,ParserContext>::EvaluatorContext;
					
					using EvaluatorFunction=std::function<BitOriginVector(const vector<BitOriginVector>&,const vector<int>&)>;
					
					class Operation
					{
						public:
						
						enum class Type{none,unaryOperator,binaryOperator,ternaryOperator,bracketOperator,bracketFunctionOperator};
						
						Type type=Type::none;
						ParserFunction parserFunction;
						EvaluatorFunction evaluatorFunction;
						
						Operation(){}
						Operation(const Type& _type,const ParserFunction& _parserFunction,const EvaluatorFunction& _evaluatorFunction)
						{
							type=_type;
							parserFunction=_parserFunction;
							evaluatorFunction=_evaluatorFunction;
						}
					};
					
					class Parser
					{
						private:
							ExpressionEvaluator<InputExpression,ParserContext> expressionEvaluator;
							
							using Function=ExpressionEvaluator<InputExpression,ParserContext>::Function;
							using EvaluatorContext=ExpressionEvaluator<InputExpression,ParserContext>::EvaluatorContext;
							
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
										int variableIndex=findWithName(context.variables,token);
										if(variableIndex==-1) throw string()+"variable named '"+token+"' not found";
										
										const Component::Variable& variable=context.variables[variableIndex];
										if(variable.type==Component::Variable::Type::output)
										{
											throw string()+"reading from output variable: '"+token+"'";
										}
										
										return InputExpression(variable,variableIndex);
									}
								};
							vector<Function> unaryOperators=vector<Function>{
								Function("-",[this](const vector<InputExpression>& args)->InputExpression
								{
									if(!args[0].isIntegerConstant()) throw string()+"expected integer for sign change";
									
									int a=args[0].integerConstantValue;
									
									int r=(-SafeInteger(a)).getValue();
									
									return InputExpression(r);
								})
							};
							vector<Function> binaryOperators=vector<Function>{};
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
							vector<Function> ternaryOperators=vector<Function>{};
							vector<Function> bracketFunctionOperators=vector<Function>{};
							vector<Function> bracketOperators=vector<Function>{
								Function(vector<string>{"(",")"},[this](const vector<InputExpression>& args)->InputExpression
								{
									return args[0];
								},1)
							};
							
							for(int i=0;i<operations.size();i++)
							{
								Operation& op=operations[i];
								if(op.type==Operation::Type::unaryOperator) unaryOperators.push_back(op.parserFunction);
								else if(op.type==Operation::Type::binaryOperator) binaryOperators.push_back(op.parserFunction);
								else if(op.type==Operation::Type::ternaryOperator) ternaryOperators.push_back(op.parserFunction);
								else if(op.type==Operation::Type::bracketFunctionOperator) bracketFunctionOperators.push_back(op.parserFunction);
								else if(op.type==Operation::Type::bracketOperator) bracketOperators.push_back(op.parserFunction);
							}
							
							expressionEvaluator.setTokenResolutionFunction(tokenResolutionFunction);
							expressionEvaluator.setUnaryOperators(unaryOperators);
							expressionEvaluator.setBinaryOperators(binaryOperators,binaryOperatorLevels);
							expressionEvaluator.setTernaryOperators(ternaryOperators);
							expressionEvaluator.setBracketFunctionOperators(bracketFunctionOperators);
							expressionEvaluator.setBracketOperators(bracketOperators);
						}
						
						InputExpression evaluate(const string& expressionString,const vector<Component::Variable>& variables)
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
					int operationIndex=0;
					
					addOperation(operationIndex,
						Operation(
							Operation::Type::bracketFunctionOperator,
							ParserFunction(vector<string>{"[",":","]"},[this,operationIndex](const vector<InputExpression>& args)->InputExpression
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
							ParserFunction("*",[this,operationIndex](const vector<InputExpression>& args)->InputExpression
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
							ParserFunction("%",[this,operationIndex](const vector<InputExpression>& args)->InputExpression
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
							ParserFunction("+",[this,operationIndex](const vector<InputExpression>& args)->InputExpression
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
				
				InputExpression parse(const string& expressionString,const vector<Component::Variable>& variables,int expectedSize)
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
			
			InputExpression parseInputExpression(const string& expressionString,const vector<Component::Variable>& variables,int expectedSize,int lineIndex)
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
						}
					}
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
						Component::Variable& variable=component.variables[v];
						
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
				
				vector<ComputerData::Input> computerInputs;
				vector<ComputerData::Input> computerMemoryInputs;
			};
			
			class ComponentContext
			{
				public:
				
				int level=0;
				
				vector<vector<ComputerData::Input>> inputs;
				
				vector<vector<ComputerData::Input>> outputs;
				
				vector<VariableData> variables;
				
				vector<int> lineIndexes;
				
				ComponentContext(){}
				ComponentContext(const ComponentContext& parent,const vector<vector<ComputerData::Input>>& _inputs,int elementLineIndex)
				{
					level=parent.level+1;
					
					inputs=_inputs;
					
					lineIndexes=parent.lineIndexes;
					lineIndexes.push_back(elementLineIndex);
				}
			};
			
			vector<ComputerData::Input> getComputerInputs(const Component& component,const ComponentContext& context,
				const Component::Element& element,const Component::Element::Input& elementInput)
			{
				vector<ComputerData::Input> computerInputs;
				
				InputExpressionEvaluator::BitOriginVector bitOriginVector=evaluateInputExpression(elementInput.expression.get(),element.lineIndex);
				
				for(int i=0;i<bitOriginVector.content.size();i++)
				{
					InputExpressionEvaluator::BitOrigin& bitOrigin=bitOriginVector.content[i];
					
					if(bitOrigin.type==InputExpressionEvaluator::BitOrigin::Type::constant)
					{
						int bit=(uint32_t(bitOrigin.constantValue))&1;
						computerInputs.emplace_back(ComputerData::Input::Type::constant,bit);
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
			
			void setVariableInputs(ComputerData& computer,ComponentContext& context,int variableIndex,const vector<ComputerData::Input>& inputs)
			{
				vector<ComputerData::Input>& computerInputs=context.variables[variableIndex].computerInputs;
				for(int i=0;i<computerInputs.size();i++)
				{
					ComputerData::Input& input=computerInputs[i];
					
					if(input.type==ComputerData::Input::Type::yesGate)
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
					Component::Variable& variable=component.variables[i];
					
					{
						VariableData variableData;
						for(int j=0;j<variable.sizeInBits;j++)
						{
							variableData.computerInputs.emplace_back(ComputerData::Input::Type::yesGate,computer.addYesGate(ComputerData::YesGate()));
						}
						context.variables.push_back(variableData);
					}
					
					if(variable.type==Component::Variable::Type::input)
					{
						vector<ComputerData::Input> inputs;
						
						if(context.level==0)
						{
							int index=computer.addInputs(variable.sizeInBits);
							
							for(int j=0;j<variable.sizeInBits;j++)
							{
								inputs.emplace_back(ComputerData::Input::Type::computerInput,index+j);
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
						vector<ComputerData::Input> inputs;
						
						for(int j=0;j<variable.sizeInBits;j++)
						{
							int index=computer.addMemory(ComputerData::Input());
							inputs.emplace_back(ComputerData::Input::Type::computerMemory,index);
						}
						
						context.variables[i].computerMemoryInputs=inputs;
					}
				}
				
				for(int e=0;e<component.elements.size();e++)
				{
					Component::Element& element=component.elements[e];
					
					if(element.type==Component::Element::Type::nand)
					{
						vector<ComputerData::Input> inputsA=getComputerInputs(component,context,element,element.inputs[0]);
						vector<ComputerData::Input> inputsB=getComputerInputs(component,context,element,element.inputs[1]);
						
						vector<ComputerData::Input> inputsC;
						
						vector<int> lineIndexes=context.lineIndexes;
						lineIndexes.push_back(element.lineIndex);
						
						for(int i=0;i<inputsA.size() && i<inputsB.size();i++)
						{
							int nandGateIndex=computer.addNandGate(ComputerData::NandGate(inputsA[i],inputsB[i]));
							
							inputsC.emplace_back(ComputerData::Input::Type::nandGate,nandGateIndex);
							
							addLineIndexesToNandGate(computer,nandGateIndex,lineIndexes);
						}
						
						setVariableInputs(computer,context,element.outputs[0].variableIndex,inputsC);
					}
					else if(element.type==Component::Element::Type::set)
					{
						vector<ComputerData::Input> inputs=getComputerInputs(component,context,element,element.inputs[0]);
						
						setVariableInputs(computer,context,element.outputs[0].variableIndex,inputs);
					}
					else if(element.type==Component::Element::Type::concat)
					{
						vector<ComputerData::Input> inputsA=getComputerInputs(component,context,element,element.inputs[0]);
						vector<ComputerData::Input> inputsB=getComputerInputs(component,context,element,element.inputs[1]);
						
						vector<ComputerData::Input> inputsC;
						
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
						vector<vector<ComputerData::Input>> inputs;
						for(int i=0;i<element.inputs.size();i++)
						{
							inputs.push_back(getComputerInputs(component,context,element,element.inputs[i]));
						}
						
						vector<vector<ComputerData::Input>> outputs(element.outputs.size());
						
						for(int i=0;i<element.numberOfInstances;i++)
						{
							vector<vector<ComputerData::Input>> instanceInputs(inputs.size());
							for(int j=0;j<inputs.size();j++)
							{
								instanceInputs[j]=vector<ComputerData::Input>(
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
					Component::Variable& variable=component.variables[i];
					
					if(variable.type==Component::Variable::Type::reg)
					{
						for(int j=0;j<context.variables[i].computerInputs.size();j++)
						{
							computer.setMemory(context.variables[i].computerMemoryInputs[j].index,context.variables[i].computerInputs[j]);
						}
					}
					else if(variable.type==Component::Variable::Type::output)
					{
						vector<ComputerData::Input> inputs=context.variables[i].computerInputs;
						
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
			void redirectThroughYesGates(const ComputerData& computer,ComputerData::Input& input)
			{
				while(input.type==ComputerData::Input::Type::yesGate)
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
			void reorderNandGates(ComputerData& computer)
			{
				vector<int> order(computer.nandGates.size(),-1);
				int orderedCount=0;
				
				for(;;)
				{
					int orderedCountOld=orderedCount;
					for(int i=0;i<computer.nandGates.size();i++)
					{
						if(order[i]==-1)
						{
							ComputerData::NandGate& nandGate=computer.nandGates[i];
							if(nandGate.inputA.type==ComputerData::Input::Type::nandGate)
							{
								if(order[nandGate.inputA.index]==-1) continue;
							}
							if(nandGate.inputB.type==ComputerData::Input::Type::nandGate)
							{
								if(order[nandGate.inputB.index]==-1) continue;
							}
							
							order[i]=orderedCount;
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
							if(order[i]==-1)
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
				
				vector<ComputerData::NandGate> newNandGates(computer.nandGates.size());
				for(int i=0;i<computer.nandGates.size();i++)
				{
					ComputerData::NandGate nandGate=computer.nandGates[i];
					
					if(nandGate.inputA.type==ComputerData::Input::Type::nandGate)
					{
						nandGate.inputA.index=order[nandGate.inputA.index];
					}
					if(nandGate.inputB.type==ComputerData::Input::Type::nandGate)
					{
						nandGate.inputB.index=order[nandGate.inputB.index];
					}
					
					newNandGates[order[i]]=nandGate;
				}
				
				computer.nandGates=newNandGates;
				
				for(int i=0;i<computer.memory.size();i++)
				{
					ComputerData::OutputGate& memory=computer.memory[i];
					
					if(memory.input.type==ComputerData::Input::Type::nandGate)
					{
						memory.input.index=order[memory.input.index];
					}
				}
				for(int i=0;i<computer.outputs.size();i++)
				{
					ComputerData::OutputGate& output=computer.outputs[i];
					
					if(output.input.type==ComputerData::Input::Type::nandGate)
					{
						output.input.index=order[output.input.index];
					}
				}
			}
			void compileComponents(ComputerData& computer)
			{
				int mainComponentIndex=findWithName(components,"main");
				if(mainComponentIndex==-1) throw string("Error: 'main' component not found");
				
				ComponentContext context;
				
				compileComponent(computer,mainComponentIndex,context);
				
				optimizeYesGatesAway(computer);
				
				reorderNandGates(computer);
			}
		public:
		Computer compile()
		{
			processTemplates();
			
			readComponents();
			
			processComponents();
			
			checkRecursivity();
			
			ComputerData computer;
			compileComponents(computer);
			
			return computer.getComputer();
		}
	};
	
	Computer buildComputer(const string& codeText)
	{
		Code code(codeText);
		Computer computer=code.compile();
		
		string trace;
		if(!computer.checkValidity(trace))
		{
			throw string("Computer not valid. Showing trace:\n")+trace;
		}
		
		return computer;
	}
};
