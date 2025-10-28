class ComputerBuilder
{
	public:
	
	class Code
	{
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
				
				Variable(){}
				Variable(const Type& _type,const string& _name,int _sizeInBits)
				{
					type=_type;
					name=_name;
					sizeInBits=_sizeInBits;
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
				
				Element(){}
				Element(const Type& _type,const string& _componentName,int _componentIndex,const vector<Output>& _outputs,const vector<Input>& _inputs)
				{
					type=_type;
					componentName=_componentName;
					componentIndex=_componentIndex;
					outputs=_outputs;
					inputs=_inputs;
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
				lines.push_back(outputLine);
			}
			string errorString(int errorCode,int lineIndex)
			{
				return string("ERROR_")+std::to_string(errorCode)+" at line "+std::to_string(lineIndex+1);
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
			int findWithName(const vector<T>& v,const string& name)
			{
				for(int i=0;i<v.size();i++)
				{
					if(v[i].name==name) return i;
				}
				return -1;
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
			vector<string> splitStringAtColons(const string& line)
			{
				vector<string> strs;
				
				string str;
				for(size_t p=0;p<line.size();p++)
				{
					if(line[p]==':')
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
				if(!isValidIdentifier(name)) throw errorString(__LINE__,lineIndex);
				
				size_t position=colon+1;
				if(position>=line.size()) throw errorString(__LINE__,lineIndex);
				vector<string> splitted=splitStringAtSpaces(line.substr(position,line.size()-position));
				
				if(splitted.size()==0 || splitted.size()>2) throw errorString(__LINE__,lineIndex);
				string typeString=splitted[0];
				int sizeInBits=1;
				if(splitted.size()>1)
				{
					try
					{
						string str=splitted[1];
						sizeInBits=std::stoi(str);
						if(sizeInBits<=0 || sizeInBits>65536) throw 1;
						if(std::to_string(sizeInBits)!=str) throw 1;
					}
					catch(...)
					{
						throw errorString(__LINE__,lineIndex);
					}
				}
				
				if(findWithName(components.back().variables,name)!=-1) throw errorString(__LINE__,lineIndex);
				
				Component::Variable::Type type=Component::Variable::Type::input;
				if(typeString=="in"){}
				else if(typeString=="out") type=Component::Variable::Type::output;
				else if(typeString=="reg") type=Component::Variable::Type::reg;
				else throw errorString(__LINE__,lineIndex);
				
				components.back().variables.emplace_back(type,name,sizeInBits);
				
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
						if(!isValidIdentifier(name)) throw errorString(__LINE__,lineIndex);
						if(findWithName(components,name)!=-1) throw errorString(__LINE__,lineIndex);
						
						
						if(components.size()>0) components.back().lastLine=lineIndex-1;
						
						components.emplace_back(name);
						
						components.back().firstLine=lineIndex;
						components.back().lastLine=lines.size()-1;
					}
					else
					{
						if(components.size()==0) throw errorString(__LINE__,lineIndex);
						
						if(isVariableDeclarationLine(line))
						{
							processVariableDeclarationLine(line,lineIndex);
						}
					}
				}
			}
			
			bool stringToIntSimple(const string& str,int& value)
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
			int getSetSizeInBits(const string& setString)
			{
				string name="set";
				if(setString.size()>=name.size())
				{
					if(setString.substr(0,name.size())==name)
					{
						if(setString.size()==name.size()) return 1;
						
						int value=0;
						if(stringToIntSimple(setString.substr(name.size(),setString.size()-name.size()),value))
						{
							if(value>0 && value<=65536) return value;
						}
					}
				}
				return -1;
			}
			bool getConcatSizeInBits(const string& concatString,int& sizeA,int& sizeB)
			{
				string name="concat";
				if(concatString.size()>=name.size())
				{
					if(concatString.substr(0,name.size())==name)
					{
						if(concatString.size()==name.size()) return false;
						
						string sizesStr=concatString.substr(name.size(),concatString.size()-name.size());
						
						size_t underscore=sizesStr.find_first_of("_");
						if(underscore==string::npos) return false;
						
						if(!stringToIntSimple(sizesStr.substr(0,underscore),sizeA)) return false;
						if(sizeA<=0 || sizeA>65536) return false;
						
						if(!stringToIntSimple(sizesStr.substr(underscore+1,sizesStr.size()-(underscore+1)),sizeB)) return false;
						if(sizeB<=0 || sizeB>65536) return false;
						
						return true;
					}
				}
				return false;
			}
			
			void processComponents()
			{
				for(int thisComponentIndex=0;thisComponentIndex<components.size();thisComponentIndex++)
				{
					Component& component=components[thisComponentIndex];
					
					for(int lineIndex=component.firstLine;lineIndex<=component.lastLine;lineIndex++)
					{
						string line=lines[lineIndex];
						
						if(line.size()==0) continue;
						
						if(!isVariableDeclarationLine(line))
						{
							size_t equalSign=line.find_first_of("=");
							if(equalSign==string::npos) throw errorString(__LINE__,lineIndex);
							
							vector<string> outputs=splitStringAtSpaces(line.substr(0,equalSign));
							if(outputs.size()==0) throw errorString(__LINE__,lineIndex);
							
							size_t position=equalSign+1;
							if(position>=line.size()) throw errorString(__LINE__,lineIndex);
							vector<string> inputs=splitStringAtSpaces(line.substr(position,line.size()-position));
							
							if(inputs.size()<=1) throw errorString(__LINE__,lineIndex);
							
							string componentName=inputs[0];
							inputs.erase(inputs.begin());
							
							if(!isValidIdentifier(componentName)) throw errorString(__LINE__,lineIndex);
							
							
							bool isNand=false;
							bool isSet=false;
							bool isConcat=false;
							bool isComponent=false;
							
							vector<int> outputSizes;
							vector<int> inputSizes;
							
							int componentIndex=-1;
							
							if(componentName=="nand")
							{
								isNand=true;
								outputSizes=vector<int>{1};
								inputSizes=vector<int>{1,1};
							}
							else
							{
								int set_sizeInBits=getSetSizeInBits(componentName);
								if(set_sizeInBits!=-1)
								{
									isSet=true;
									outputSizes=vector<int>{set_sizeInBits};
									inputSizes=vector<int>{set_sizeInBits};
								}
								else
								{
									int concat_sizeA=-1;
									int concat_sizeB=-1;
									if(getConcatSizeInBits(componentName,concat_sizeA,concat_sizeB))
									{
										isConcat=true;
										outputSizes=vector<int>{concat_sizeA+concat_sizeB};
										inputSizes=vector<int>{concat_sizeA,concat_sizeB};
									}
									else
									{
										isComponent=true;
										
										componentIndex=findWithName(components,componentName);
										if(componentIndex==-1 || componentIndex==thisComponentIndex) throw errorString(__LINE__,lineIndex);
										
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
							
							if(inputs.size()!=inputSizes.size()) throw errorString(__LINE__,lineIndex);
							if(outputs.size()!=outputSizes.size()) throw errorString(__LINE__,lineIndex);
							
							vector<int> outputVariableIndexes;
							
							for(int i=0;i<outputs.size();i++)
							{
								string name=outputs[i];
								if(!isValidIdentifier(name)) throw errorString(__LINE__,lineIndex);
								
								int variableIndex=findWithName(component.variables,name);
								if(variableIndex!=-1)
								{
									Component::Variable::Type variableType=component.variables[variableIndex].type;
									if(variableType!=Component::Variable::Type::output && variableType!=Component::Variable::Type::reg)
									{
										throw errorString(__LINE__,lineIndex);
									}
									
									outputVariableIndexes.push_back(variableIndex);
								}
								else
								{
									outputVariableIndexes.push_back(component.variables.size());
									
									component.variables.emplace_back(Component::Variable::Type::intermediate,name,outputSizes[i]);
								}
							}
							
							
							Component::Element::Type elementType=Component::Element::Type::component;
							if(isNand) elementType=Component::Element::Type::nand;
							else if(isSet) elementType=Component::Element::Type::set;
							else if(isConcat) elementType=Component::Element::Type::concat;
							else if(isComponent){}
							
							vector<Component::Element::Output> elementOutputs;
							for(int i=0;i<outputVariableIndexes.size();i++)
							{
								elementOutputs.emplace_back(outputVariableIndexes[i]);
							}
							
							vector<Component::Element::Input> elementInputs;
							for(int i=0;i<inputs.size();i++)
							{
								string str=inputs[i];
								
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
											throw errorString(__LINE__,lineIndex);
										}
										
										elementInputs.emplace_back(true,constant,0,inputSizes[i]);
										
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
									
									if(str.back()!=']') throw errorString(__LINE__,lineIndex);
									
									string bracketContent=str.substr(bracket+1,(str.size()-1)-(bracket+1));
									
									vector<string> numbers=splitStringAtColons(bracketContent);
									
									if(numbers.size()==0) throw errorString(__LINE__,lineIndex);
									
									if(numbers[0].size()==0) throw errorString(__LINE__,lineIndex);
									if(!stringToIntSimple(numbers[0],offset)) throw errorString(__LINE__,lineIndex);
									if(offset<0) throw errorString(__LINE__,lineIndex);
									
									size=1;
									if(numbers.size()>1)
									{
										int offsetB=0;
										if(numbers[1].size()==0) throw errorString(__LINE__,lineIndex);
										if(!stringToIntSimple(numbers[1],offsetB)) throw errorString(__LINE__,lineIndex);
										
										size=offsetB-offset;
										
										if(size<=0) throw errorString(__LINE__,lineIndex);
									}
									
									if(numbers.size()>2) throw errorString(__LINE__,lineIndex);
								}
								
								if(!isValidIdentifier(name)) throw errorString(__LINE__,lineIndex);
								
								int variableIndex=findWithName(component.variables,name);
								if(variableIndex==-1) throw errorString(__LINE__,lineIndex);
								
								int variableSize=component.variables[variableIndex].sizeInBits;
								if(size==-1) size=variableSize;
								
								
								if(offset+size>variableSize) throw errorString(__LINE__,lineIndex);
								
								if(size!=inputSizes[i]) throw errorString(__LINE__,lineIndex);
								
								
								if(component.variables[variableIndex].type==Component::Variable::Type::output)
								{
									throw errorString(__LINE__,lineIndex);
								}
								
								
								elementInputs.emplace_back(false,variableIndex,offset,inputSizes[i]);
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
											throw errorString(__LINE__,lineIndex);
										}
									}
								}
							}
							
							component.elements.emplace_back(
								elementType,
								elementType==Component::Element::Type::component ? componentName : string(),
								componentIndex,
								elementOutputs,
								elementInputs
								);
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
							
							str+=components[i].name+" (line "+std::to_string(components[i].firstLine+1)+")";
							
							count++;
						}
					}
					
					throw string("Error: Cannot compile certain components due to recursivity: ")+str;
				}
			}
			
			class VariableData
			{
				public:
				
				vector<Computer::Input> computerInputs;
				vector<Computer::Input> computerMemoryInputs;
				
				VariableData(){}
				explicit VariableData(int sizeInBits)
				{
					computerInputs=vector<Computer::Input>(sizeInBits);
					computerMemoryInputs=vector<Computer::Input>(sizeInBits);
				}
			};
			
			class ComponentContext
			{
				public:
				
				int level=0;
				
				vector<vector<Computer::Input>> inputs;
				
				vector<vector<Computer::Input>> outputs;
				
				vector<VariableData> variables;
				
				ComponentContext(){}
				ComponentContext(const ComponentContext& parent,const vector<vector<Computer::Input>>& _inputs)
				{
					level=parent.level+1;
					
					inputs=_inputs;
				}
			};
			
			vector<Computer::Input> getComputerInputs(const ComponentContext& context,const Component::Element::Input& elementInput)
			{
				vector<Computer::Input> computerInputs;
				
				if(elementInput.isConstant)
				{
					for(int i=0;i<elementInput.sizeInBits;i++)
					{
						computerInputs.emplace_back(Computer::Input::Type::constant,(uint64_t(elementInput.constantValue)>>i)&1);
					}
				}
				else
				{
					const VariableData& variable=context.variables[elementInput.variableIndex];
					
					for(int i=0;i<elementInput.sizeInBits;i++)
					{
						computerInputs.push_back(variable.computerInputs[elementInput.variableOffsetInBits+i]);
					}
				}
				
				return computerInputs;
			}
			
			void compileComponent(Computer& computer,int thisComponentIndex,ComponentContext& context)
			{
				Component& component=components[thisComponentIndex];
				
				for(int i=0;i<component.variables.size();i++)
				{
					Component::Variable& variable=component.variables[i];
					
					context.variables.emplace_back(variable.sizeInBits);
				}
				
				for(int i=0;i<component.inputs.size();i++)
				{
					vector<Computer::Input> inputs;
					
					if(context.level==0)
					{
						int size=component.inputs[i].sizeInBits;
						int inputIndex=computer.addInputs(size);
						
						for(int j=0;j<size;j++)
						{
							inputs.emplace_back(Computer::Input::Type::computerInput,inputIndex+j);
						}
					}
					else
					{
						inputs=context.inputs[i];
					}
					
					int variableIndex=findWithName(component.variables,component.inputs[i].name);
					context.variables[variableIndex].computerInputs=inputs;
				}
				
				for(int i=0;i<component.variables.size();i++)
				{
					Component::Variable& variable=component.variables[i];
					
					if(variable.type==Component::Variable::Type::reg)
					{
						vector<Computer::Input> inputs;
						
						int size=variable.sizeInBits;
						
						for(int j=0;j<size;j++)
						{
							int index=computer.addMemory(Computer::Input());
							inputs.emplace_back(Computer::Input::Type::computerMemory,index);
						}
						
						context.variables[i].computerMemoryInputs=inputs;
						
						context.variables[i].computerInputs=inputs;
					}
				}
				
				for(int e=0;e<component.elements.size();e++)
				{
					Component::Element& element=component.elements[e];
					
					if(element.type==Component::Element::Type::nand)
					{
						vector<Computer::Input> inputsA=getComputerInputs(context,element.inputs[0]);
						vector<Computer::Input> inputsB=getComputerInputs(context,element.inputs[1]);
						
						vector<Computer::Input> inputsC;
						
						for(int i=0;i<inputsA.size() && i<inputsB.size();i++)
						{
							int nandGateIndex=computer.addNandGate(Computer::NandGate(inputsA[i],inputsB[i]));
							
							inputsC.emplace_back(Computer::Input::Type::nandGate,nandGateIndex);
						}
						
						context.variables[element.outputs[0].variableIndex].computerInputs=inputsC;
					}
					else if(element.type==Component::Element::Type::set)
					{
						vector<Computer::Input> inputs=getComputerInputs(context,element.inputs[0]);
						
						context.variables[element.outputs[0].variableIndex].computerInputs=inputs;
					}
					else if(element.type==Component::Element::Type::concat)
					{
						vector<Computer::Input> inputs=getComputerInputs(context,element.inputs[0]);
						vector<Computer::Input> inputsToConcatenate=getComputerInputs(context,element.inputs[1]);
						
						inputs.insert(inputs.end(),inputsToConcatenate.begin(),inputsToConcatenate.end());
						
						context.variables[element.outputs[0].variableIndex].computerInputs=inputs;
					}
					else if(element.type==Component::Element::Type::component)
					{
						vector<vector<Computer::Input>> inputs;
						for(int i=0;i<element.inputs.size();i++)
						{
							inputs.push_back(getComputerInputs(context,element.inputs[i]));
						}
						
						ComponentContext subcontext(context,inputs);
						
						compileComponent(computer,element.componentIndex,subcontext);
						
						for(int i=0;i<element.outputs.size();i++)
						{
							context.variables[element.outputs[i].variableIndex].computerInputs=subcontext.outputs[i];
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
				}
				
				for(int i=0;i<component.outputs.size();i++)
				{
					int variableIndex=findWithName(component.variables,component.outputs[i].name);
					vector<Computer::Input> inputs=context.variables[variableIndex].computerInputs;
					
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
			void compileComponents(Computer& computer)
			{
				int mainComponentIndex=findWithName(components,"main");
				if(mainComponentIndex==-1) throw string("Error: 'main' component not found");
				
				ComponentContext context;
				
				compileComponent(computer,mainComponentIndex,context);
			}
		public:
		Computer compile()
		{
			readComponents();
			
			processComponents();
			
			checkRecursivity();
			
			Computer computer;
			compileComponents(computer);
			
			return computer;
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
