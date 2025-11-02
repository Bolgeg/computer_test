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
		public:
		
		static constexpr int maximumVariableSizeInBits=(1<<30);
		
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
					
					int isConstant=true;
					int constantValue=0;
					
					int variableIndex=-1;
					int variableOffsetInBits=0;
					
					int sizeInBits=1;
					
					Input(){}
					Input(bool _isConstant,int variableIndexOrConstantValue,int offset,int size)
					{
						isConstant=_isConstant;
						
						if(isConstant) constantValue=variableIndexOrConstantValue;
						else variableIndex=variableIndexOrConstantValue;
						
						variableOffsetInBits=offset;
						sizeInBits=size;
					}
				};
				
				enum class Type{nand,set,concat,component};
				
				Type type=Type::nand;
				
				string componentName;
				int componentIndex=-1;
				
				vector<Output> outputs;
				vector<Input> inputs;
				
				int lineIndex=-1;
				
				Element(){}
				Element(const Type& _type,const string& _componentName,int _componentIndex,const vector<Output>& _outputs,const vector<Input>& _inputs,
					int _lineIndex)
				{
					type=_type;
					componentName=_componentName;
					componentIndex=_componentIndex;
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
			
			class TemplateExpressionEvaluator
			{
				public:
				
				private:
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
							static bool isDigit(uint8_t c)
							{
								return c>='0' && c<='9';
							}
						public:
						
						static bool isWordToken(const string& token)
						{
							return token.size()>0 && isCharacterOfWordToken(token[0]);
						}
						
						static bool isIdentifierToken(const string& token)
						{
							return token.size()>0 && isCharacterOfWordToken(token[0]) && !isDigit(token[0]);
						}
						static bool isNumberToken(const string& token)
						{
							return token.size()>0 && isDigit(token[0]);
						}
					};
					
					static string errorString(int errorCode)
					{
						return string("ERROR_")+std::to_string(errorCode);
					}
					static string errorString(const string& errorMessage)
					{
						return errorMessage;
					}
					
					string getTemplateArgumentWithName(const string& name,const vector<TemplateArgument>& templateArguments)
					{
						int argumentIndex=findWithName(templateArguments,name);
						if(argumentIndex==-1) throw errorString(string()+"Variable named '"+name+"' not found");
						
						return templateArguments[argumentIndex].value;
					}
					
					class Function
					{
						public:
						
						string name;
						vector<string> nameTokens;
						
						std::function<string(const vector<string>&)> function;
						
						int numberOfArguments=-1;
						
						Function(){}
						Function(const string& _name,const std::function<string(const vector<string>&)>& _function,int _numberOfArguments=-1)
						{
							name=_name;
							nameTokens=Code::tokenize(name);
							
							function=_function;
							
							numberOfArguments=_numberOfArguments;
						}
						
						void setFixedNumberOfArguments(int _numberOfArguments)
						{
							numberOfArguments=_numberOfArguments;
						}
						string execute(const vector<string>& args)
						{
							if(numberOfArguments!=-1)
							{
								if(args.size()!=numberOfArguments) throw errorString(wrongNumberOfArgumentsMessage(name));
							}
							return function(args);
						}
					};
					
					void setFixedNumberOfArguments(vector<Function>& functionsToModify,int numberOfArguments)
					{
						for(int f=0;f<functionsToModify.size();f++)
						{
							functionsToModify[f].setFixedNumberOfArguments(numberOfArguments);
						}
					}
					
					static string wrongNumberOfArgumentsMessage(const string& functionName)
					{
						return string()+"Wrong number of arguments for function '"+functionName+"'";
					}
					static int stringToInt(const string& str)
					{
						int intValue=0;
						if(!stringToIntSimple(str,intValue)) throw errorString(string()+"Could not convert the string '"+str+"' to integer");
						return intValue;
					}
					static string intToString(int intValue)
					{
						return std::to_string(intValue);
					}
					static void assertDivision(int dividend,int divisor)
					{
						if(divisor==0 || dividend==int(uint32_t(1)<<31) && divisor==-1)
						{
							throw errorString(string()+"Division/modulo operation of invalid integers: "+std::to_string(dividend)+" and "+std::to_string(divisor));
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
					
					vector<Function> unaryOperators=vector<Function>
					{
						Function("-",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							
							if(uint32_t(a)==(uint32_t(1)<<31)) throw errorString(string()+"sign change out of range: "+"-"+std::to_string(a));
							int r=-a;
							
							return intToString(r);
						}),
						Function("~",[this](const vector<string>& args)->string
						{
							return intToString(~stringToInt(args[0]));
						}),
						Function("!",[this](const vector<string>& args)->string
						{
							return intToString(!stringToInt(args[0]));
						})
					};
					
					vector<Function> binaryOperators=vector<Function>
					{
						Function("**",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							int b=stringToInt(args[1]);
							
							int r=0;
							if(a==0 && b<=0) throw errorString(string()+"0 to the power of "+std::to_string(b));
							
							if(a==1) r=1;
							else if(a==-1) r=(1-int(uint32_t(b)&1)*2);
							else if(a!=0 && b>=0)
							{
								r=1;
								for(int i=0;i<b;i++)
								{
									if(multiplicationOverflows(r,a)) throw errorString(string()+"Exponentiation result out of range: "+std::to_string(a)+"**"+std::to_string(b));
									r*=a;
								}
							}
							
							return intToString(r);
						}),
						Function("*",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							int b=stringToInt(args[1]);
							
							if(multiplicationOverflows(a,b)) throw errorString(string()+"Multiplication result out of range: "+std::to_string(a)+"*"+std::to_string(b));
							int r=a*b;
							
							return intToString(r);
						}),
						Function("/",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							int b=stringToInt(args[1]);
							assertDivision(a,b);
							return intToString(a/b);
						}),
						Function("%",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							int b=stringToInt(args[1]);
							assertDivision(a,b);
							return intToString(a%b);
						}),
						Function("+",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							int b=stringToInt(args[1]);
							
							if(additionOverflows(a,b)) throw errorString(string()+"Addition overflow: "+std::to_string(a)+"+"+std::to_string(b));
							int r=a+b;
							
							return intToString(r);
						}),
						Function("-",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							int b=stringToInt(args[1]);
							
							if(subtractionOverflows(a,b)) throw errorString(string()+"Subtraction overflow: "+std::to_string(a)+"-"+std::to_string(b));
							int r=a-b;
							
							return intToString(r);
						}),
						Function("<<",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							int b=stringToInt(args[1]);
							if(b<0) throw errorString(string()+"Negative bit shift: "+std::to_string(a)+"<<"+std::to_string(b));
							int c= b<32 ? (a>>b) : 0;
							return intToString(c);
						}),
						Function(">>",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							int b=stringToInt(args[1]);
							if(b<0) throw errorString(string()+"Negative bit shift: "+std::to_string(a)+">>"+std::to_string(b));
							int c= b<32 ? (a>>b) : (a<0 ? -1 : 0);
							return intToString(c);
						}),
						Function(">>>",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							int b=stringToInt(args[1]);
							if(b<0) throw errorString(string()+"Negative bit shift: "+std::to_string(a)+">>>"+std::to_string(b));
							int c= b<32 ? int(uint32_t(a)>>b) : 0;
							return intToString(c);
						}),
						Function("<",[this](const vector<string>& args)->string
						{
							return intToString(stringToInt(args[0])<stringToInt(args[1]));
						}),
						Function("<=",[this](const vector<string>& args)->string
						{
							return intToString(stringToInt(args[0])<=stringToInt(args[1]));
						}),
						Function(">",[this](const vector<string>& args)->string
						{
							return intToString(stringToInt(args[0])>stringToInt(args[1]));
						}),
						Function(">=",[this](const vector<string>& args)->string
						{
							return intToString(stringToInt(args[0])>=stringToInt(args[1]));
						}),
						Function("==",[this](const vector<string>& args)->string
						{
							return intToString(stringToInt(args[0])==stringToInt(args[1]));
						}),
						Function("!=",[this](const vector<string>& args)->string
						{
							return intToString(stringToInt(args[0])!=stringToInt(args[1]));
						}),
						Function("&",[this](const vector<string>& args)->string
						{
							return intToString(uint32_t(stringToInt(args[0]))&uint32_t(stringToInt(args[1])));
						}),
						Function("^",[this](const vector<string>& args)->string
						{
							return intToString(uint32_t(stringToInt(args[0]))^uint32_t(stringToInt(args[1])));
						}),
						Function("|",[this](const vector<string>& args)->string
						{
							return intToString(uint32_t(stringToInt(args[0]))|uint32_t(stringToInt(args[1])));
						}),
						Function("&&",[this](const vector<string>& args)->string
						{
							return intToString(stringToInt(args[0])&&stringToInt(args[1]));
						}),
						Function("||",[this](const vector<string>& args)->string
						{
							return intToString(stringToInt(args[0])||stringToInt(args[1]));
						})
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
					
					Function ternaryOperator=Function("?",[this](const vector<string>& args)->string
					{
						return stringToInt(args[0])?args[1]:args[2];
					});
					
					vector<Function> functions=vector<Function>{
						Function("isp2",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							int r=a>0 && std::has_single_bit(uint32_t(a));
							
							return intToString(r);
						},1),
						Function("log2",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							if(a<=0) throw errorString(string()+"Logarithm of zero or negative number: "+std::to_string(a));
							int r=int(std::countr_zero(std::bit_floor(uint32_t(a))));
							
							return intToString(r);
						},1),
						Function("p2floor",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							if(a<=0) throw errorString(string()+"invalid zero or negative number for function: "+std::to_string(a));
							int r=int(std::bit_floor(uint32_t(a)));
							
							return intToString(r);
						},1),
						Function("p2ceil",[this](const vector<string>& args)->string
						{
							int a=stringToInt(args[0]);
							if(a<=0 || a>(1<<30)) throw errorString(string()+"invalid out of range number for function: "+std::to_string(a));
							int r=int(std::bit_ceil(uint32_t(a)));
							
							return intToString(r);
						},1)
					};
				public:
				
				TemplateExpressionEvaluator()
				{
					setFixedNumberOfArguments(unaryOperators,1);
					setFixedNumberOfArguments(binaryOperators,2);
					ternaryOperator.setFixedNumberOfArguments(3);
				}
				
				private:
					int getBinaryOperatorLevel(const string& op)
					{
						for(int i=0;i<binaryOperatorLevels.size();i++)
						{
							for(int j=0;j<binaryOperatorLevels[i].size();j++)
							{
								if(op==binaryOperatorLevels[i][j])
								{
									return binaryOperatorLevels.size()-i;
								}
							}
						}
						return 0;
					}
					int getMaximumBinaryOperatorLevel()
					{
						return binaryOperatorLevels.size()+1;
					}
					
					bool parseOperator(const Code& code,size_t& tokenIndex,string& outputOp,const vector<Function>& operators)
					{
						size_t t=tokenIndex;
						vector<int> matches(operators.size(),true);
						vector<string> candidate;
						for(;;)
						{
							if(t>=code.tokens.size()) break;
							
							string token=code.tokens[t];
							
							candidate.push_back(token);
							
							int matchIndex=-1;
							for(int o=0;o<matches.size();o++)
							{
								if(!matches[o]) continue;
								
								if(operators[o].nameTokens.size()<candidate.size()) matches[o]=false;
								else matches[o]= operators[o].nameTokens[candidate.size()-1]==candidate[candidate.size()-1];
								
								if(matches[o]) matchIndex=o;
							}
							
							if(matchIndex==-1)
							{
								candidate.resize(candidate.size()-1);
								break;
							}
							
							t++;
						}
						
						if(candidate.size()==0) return false;
						
						tokenIndex=t;
						outputOp=Code::untokenize(candidate);
						return true;
					}
					bool parseUnaryOperator(const Code& code,size_t& t,string& op)
					{
						return parseOperator(code,t,op,unaryOperators);
					}
					bool parseBinaryOperator(const Code& code,size_t& t,string& op)
					{
						return parseOperator(code,t,op,binaryOperators);
					}
					bool parseFunctionName(const Code& code,size_t& t,string& functionName)
					{
						string token=code.tokens[t];
						if(findWithName(functions,token)!=-1)
						{
							functionName=token;
							t++;
							return true;
						}
						else return false;
					}
					
					string executeUnaryOperator(const string& op,const string& a)
					{
						int index=findWithName(unaryOperators,op);
						if(index!=-1)
						{
							return unaryOperators[index].execute(vector<string>{a});
						}
						else throw errorString(__LINE__);
					}
					string executeBinaryOperator(const string& op,const string& a,const string& b)
					{
						int index=findWithName(binaryOperators,op);
						if(index!=-1)
						{
							return binaryOperators[index].execute(vector<string>{a,b});
						}
						else throw errorString(__LINE__);
					}
					string executeTernaryOperator(const string& a,const string& b,const string& c)
					{
						return ternaryOperator.execute(vector<string>{a,b,c});
					}
					string executeFunction(const string& functionName,const vector<string>& args)
					{
						int index=findWithName(functions,functionName);
						if(index!=-1)
						{
							return functions[index].execute(args);
						}
						else throw errorString(__LINE__);
					}
					
					string expectedContinuationMessage(int errorCode)
					{
						return "Expected continuation of expression";
					}
					string expectedTokenMessage(const string& expectedToken,const string& actualToken,int errorCode)
					{
						return string()+"Expected '"+expectedToken+"' instead of '"+actualToken+"'";
					}
					string unexpectedTokenMessage(const string& token,int errorCode)
					{
						return string()+"Unexpected symbol '"+token+"'";
					}
					
					string evaluateExpressionPart(const Code& code,size_t& t,const vector<TemplateArgument>& templateArguments,int level)
					{
						if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
						
						string token=code.tokens[t];
						
						string lvalue;
						
						if(token=="(")
						{
							t++;
							if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
							lvalue=evaluateExpressionPart(code,t,templateArguments,0);
							if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
							if(code.tokens[t]!=")") throw errorString(expectedTokenMessage(")",code.tokens[t],__LINE__));
							t++;
						}
						else if(Code::isIdentifierToken(token))
						{
							string functionName;
							if(parseFunctionName(code,t,functionName))
							{
								if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
								if(code.tokens[t]!="(") throw errorString(expectedTokenMessage("(",code.tokens[t],__LINE__));
								t++;
								if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
								
								vector<string> functionArguments;
								
								for(;;)
								{
									if(code.tokens[t]==")") break;
									
									functionArguments.push_back(evaluateExpressionPart(code,t,templateArguments,0));
									
									if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
									if(code.tokens[t]==",")
									{
										t++;
										if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
									}
									else if(code.tokens[t]!=")") throw errorString(expectedTokenMessage(")",code.tokens[t],__LINE__));
								}
								
								lvalue=executeFunction(functionName,functionArguments);
								
								if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
								if(code.tokens[t]!=")") throw errorString(expectedTokenMessage(")",code.tokens[t],__LINE__));
								t++;
							}
							else
							{
								lvalue=getTemplateArgumentWithName(token,templateArguments);
								t++;
							}
						}
						else if(Code::isNumberToken(token))
						{
							lvalue=token;
							t++;
						}
						else
						{
							string op;
							if(parseUnaryOperator(code,t,op))
							{
								if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
								lvalue=executeUnaryOperator(op,evaluateExpressionPart(code,t,templateArguments,getMaximumBinaryOperatorLevel()));
							}
							else throw errorString(unexpectedTokenMessage(code.tokens[t],__LINE__));
						}
						
						for(;;)
						{
							if(t>=code.tokens.size()) break;
							token=code.tokens[t];
							if(token=="," || token==")") break;
							
							if(token=="?")
							{
								if(level>0) break;
								
								t++;
								if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
								
								string valueA=evaluateExpressionPart(code,t,templateArguments,0);
								
								if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
								if(code.tokens[t]!=":") throw errorString(expectedTokenMessage(":",code.tokens[t],__LINE__));
								t++;
								if(t>=code.tokens.size()) throw errorString(expectedContinuationMessage(__LINE__));
								
								string valueB=evaluateExpressionPart(code,t,templateArguments,0);
								
								lvalue=executeTernaryOperator(lvalue,valueA,valueB);
								
								break;
							}
							
							string op;
							size_t tCopy=t;
							if(parseBinaryOperator(code,t,op))
							{
								int operatorLevel=getBinaryOperatorLevel(op);
								if(operatorLevel>level)
								{
									if(t>=code.tokens.size()) throw 1;
									lvalue=executeBinaryOperator(op,lvalue,evaluateExpressionPart(code,t,templateArguments,operatorLevel));
								}
								else
								{
									t=tCopy;
									break;
								}
							}
							else throw errorString(unexpectedTokenMessage(code.tokens[t],__LINE__));
						}
						
						return lvalue;
					}
				public:
				
				string evaluate(const string& expressionString,const vector<TemplateArgument>& templateArguments)
				{
					Code code(expressionString);
					size_t t=0;
					return evaluateExpressionPart(code,t,templateArguments,0);
				}
			};
			
			string getTemplateArgumentExpressionResult(const string& expressionString,const vector<TemplateArgument>& templateArguments,int lineIndex)
			{
				TemplateExpressionEvaluator evaluator;
				try
				{
					return evaluator.evaluate(expressionString,templateArguments);
				}
				catch(const string& str)
				{
					throw errorString(string("Error in expression evaluation: ")+str,lineIndex);
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
							
							templateParameters.emplace_back(name,values);
						}
					}
					
					if(templateParameters.size()==0) throw errorString(__LINE__,lineIndex);
					
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
									
									bool assertMode=false;
									if(str[0]=='?')
									{
										assertMode=true;
										str=str.substr(1,str.size()-1);
									}
									
									if(templateLineIndex==0)
									{
										size_t equalSign=str.find_first_of("=");
										if(equalSign==string::npos) throw errorString(__LINE__,lineIndex);
										
										string name=str.substr(0,equalSign);
										
										int parameterIndex=findWithName(templateParameters,name);
										if(parameterIndex==-1) throw errorString(__LINE__,lineIndex);
										
										finalLine+=templateArguments[parameterIndex].value;
									}
									else
									{
										string expressionResult=getTemplateArgumentExpressionResult(str,templateArguments,lineIndex);
										
										if(assertMode)
										{
											if(expressionResult=="0")
											{
												ignoreThisCombination=true;
												break;
											}
										}
										else
										{
											finalLine+=expressionResult;
										}
									}
								}
								else
								{
									finalLine+=str;
								}
							}
						}
						
						if(ignoreThisCombination) break;
						
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
							if(line.find_first_of("<")!=string::npos)
							{
								processTemplate(originalLineIndex);
								continue;
							}
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
				
				vector<int> outputSizes;
				vector<int> inputSizes;
				
				vector<int> outputVariableIndexes;
				
				AssignmentLineData(){}
				AssignmentLineData(int _lineIndex,const vector<string>& _outputs,const string& _componentName,const vector<string>& _inputs,
					const Component::Element::Type& _type,int _componentIndex,const vector<int>& _outputSizes,const vector<int>& _inputSizes,
					const vector<int>& _outputVariableIndexes)
				{
					lineIndex=_lineIndex;
					outputs=_outputs;
					componentName=_componentName;
					inputs=_inputs;
					type=_type;
					componentIndex=_componentIndex;
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
							
							string componentName=inputs[0];
							inputs.erase(inputs.begin());
							
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
								elementType,componentIndex,outputSizes,inputSizes,outputVariableIndexes);
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
							
							if(str.size()>0)
							{
								if(str[0]>='0' && str[0]<='9' || str[0]=='-')
								{
									int constant=0;
									
									try
									{
										size_t p=0;
										constant=std::stoi(str,&p,0);
										if(p!=str.size()) throw 1;
									}
									catch(...)
									{
										throw errorString(string()+"Invalid integer constant '"+str+"'",lineIndex);
									}
									
									int inputSize=assignmentLine.inputSizes[i];
									
									if(inputSize<32)
									{
										int minimumSignedNegative=-(uint32_t(1)<<(inputSize-1));
										int maximumUnsignedPositive=(uint32_t(1)<<inputSize)-1;
										
										if(constant<minimumSignedNegative || constant>maximumUnsignedPositive)
										{
											throw errorString(string()+"Integer constant '"+str+"' out of range for this input of component",lineIndex);
										}
									}
									
									elementInputs.emplace_back(true,constant,0,inputSize);
									
									continue;
								}
							}
							
							int offset=0;
							int size=-1;
							
							string name=str;
							
							size_t bracket=str.find_first_of("[");
							if(bracket!=string::npos)
							{
								name=str.substr(0,bracket);
								
								if(str.back()!=']') throw errorString("Expected ']'",lineIndex);
								
								string bracketContent=str.substr(bracket+1,(str.size()-1)-(bracket+1));
								
								vector<string> numbers=splitStringAtColons(bracketContent);
								
								if(numbers.size()==0) throw errorString("Expected bit index or range inside '[]'",lineIndex);
								
								if(numbers[0].size()==0) throw errorString("Empty bit index",lineIndex);
								if(!stringToIntSimple(numbers[0],offset)) throw errorString(string()+"Invalid bit index '"+numbers[0]+"'",lineIndex);
								if(offset<0) throw errorString(string()+"Negative bit index: "+numbers[0],lineIndex);
								
								size=1;
								if(numbers.size()>1)
								{
									int offsetB=0;
									if(numbers[1].size()==0) throw errorString("Empty bit range end",lineIndex);
									if(!stringToIntSimple(numbers[1],offsetB)) throw errorString(string()+"Invalid bit range end '"+numbers[1]+"'",lineIndex);
									
									size=offsetB-offset;
									
									if(size<=0) throw errorString(string()+"Invalid bit range: ["+numbers[0]+":"+numbers[1]+"]",lineIndex);
								}
								
								if(numbers.size()>2) throw errorString("Too many arguments for bit range",lineIndex);
							}
							
							if(!isValidIdentifier(name)) throw errorString(string()+"Input variable name not valid: '"+name+"'",lineIndex);
							
							int variableIndex=findWithName(component.variables,name);
							if(variableIndex==-1) throw errorString(string()+"Input variable not declared: '"+name+"'",lineIndex);
							
							int variableSize=component.variables[variableIndex].sizeInBits;
							if(size==-1) size=variableSize;
							
							
							if(offset+size>variableSize)
							{
								throw errorString(string()+"Bit range out of range of variable: "
									+name+"["+std::to_string(offset)+":"+std::to_string(offset+size)+"]",lineIndex);
							}
							
							if(size!=assignmentLine.inputSizes[i])
							{
								throw errorString(string()+"Input argument size does not match the component parameter size: "
									+name+"["+std::to_string(offset)+":"+std::to_string(offset+size)+"]"
									+" -> "+std::to_string(size)+"!="+std::to_string(assignmentLine.inputSizes[i]),lineIndex);
							}
							
							
							if(component.variables[variableIndex].type==Component::Variable::Type::output)
							{
								throw errorString(string()+"Invalid use of output variable as input: '"+name+"'",lineIndex);
							}
							
							
							elementInputs.emplace_back(false,variableIndex,offset,assignmentLine.inputSizes[i]);
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
			
			vector<ComputerData::Input> getComputerInputs(const Component& component,const ComponentContext& context,const Component::Element::Input& elementInput)
			{
				vector<ComputerData::Input> computerInputs;
				
				if(elementInput.isConstant)
				{
					for(int i=0;i<elementInput.sizeInBits;i++)
					{
						int bit=0;
						if(i<64) bit=(uint64_t(elementInput.constantValue)>>i)&1;
						computerInputs.emplace_back(ComputerData::Input::Type::constant,bit);
					}
				}
				else
				{
					const Component::Variable& variable=component.variables[elementInput.variableIndex];
					const VariableData& variableData=context.variables[elementInput.variableIndex];
					
					if(variable.type==Component::Variable::Type::reg)
					{
						for(int i=0;i<elementInput.sizeInBits;i++)
						{
							computerInputs.push_back(variableData.computerMemoryInputs[elementInput.variableOffsetInBits+i]);
						}
					}
					else
					{
						for(int i=0;i<elementInput.sizeInBits;i++)
						{
							computerInputs.push_back(variableData.computerInputs[elementInput.variableOffsetInBits+i]);
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
						vector<ComputerData::Input> inputsA=getComputerInputs(component,context,element.inputs[0]);
						vector<ComputerData::Input> inputsB=getComputerInputs(component,context,element.inputs[1]);
						
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
						vector<ComputerData::Input> inputs=getComputerInputs(component,context,element.inputs[0]);
						
						setVariableInputs(computer,context,element.outputs[0].variableIndex,inputs);
					}
					else if(element.type==Component::Element::Type::concat)
					{
						vector<ComputerData::Input> inputs=getComputerInputs(component,context,element.inputs[0]);
						vector<ComputerData::Input> inputsToConcatenate=getComputerInputs(component,context,element.inputs[1]);
						
						inputs.insert(inputs.end(),inputsToConcatenate.begin(),inputsToConcatenate.end());
						
						setVariableInputs(computer,context,element.outputs[0].variableIndex,inputs);
					}
					else if(element.type==Component::Element::Type::component)
					{
						vector<vector<ComputerData::Input>> inputs;
						for(int i=0;i<element.inputs.size();i++)
						{
							inputs.push_back(getComputerInputs(component,context,element.inputs[i]));
						}
						
						ComponentContext subcontext(context,inputs,element.lineIndex);
						
						compileComponent(computer,element.componentIndex,subcontext);
						
						for(int i=0;i<element.outputs.size();i++)
						{
							setVariableInputs(computer,context,element.outputs[i].variableIndex,subcontext.outputs[i]);
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
