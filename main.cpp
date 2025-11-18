#include "base/base.cpp"

#include <filesystem>
#include <chrono>

#include "threadset.cpp"

class TimeCounter
{
	public:
	
	std::chrono::high_resolution_clock::time_point timePoint;
	
	void start()
	{
		timePoint=std::chrono::high_resolution_clock::now();
	}
	double end()
	{
		std::chrono::high_resolution_clock::time_point endTimePoint=std::chrono::high_resolution_clock::now();
		
		return double(std::chrono::duration_cast<std::chrono::microseconds>(endTimePoint-timePoint).count())/1000000;
	}
};

string fileToString(const string& filepath)
{
	binary::Binary binary;
	binary.loadFromFile(filepath);
	string str;
	str.reserve(binary.size());
	for(uint64_t i=0;i<binary.size();i++)
	{
		str.push_back(binary.content[i]);
	}
	return str;
}

void stringToFile(const string& str,const string& filepath)
{
	binary::Binary binary;
	binary.resize(str.size());
	for(uint64_t i=0;i<binary.size();i++)
	{
		binary.content[i]=str[i];
	}
	binary.saveToFile(filepath);
}

class Graph
{
	public:
	
	int lineColor=0xff0000;
	
	vector<float> lastValues;
	
	void addValue(float value)
	{
		lastValues.push_back(value);
		if(lastValues.size()>10000)
		{
			lastValues.erase(lastValues.begin());
		}
	}
	void setValues(const vector<float>& values)
	{
		lastValues=values;
	}
	void draw(graphics::Image& screen,const Pos& position,const Pos& size)
	{
		screen.rect(position,position+size,0xffffff);
		
		for(int vinv=0;vinv<size.x;vinv++)
		{
			int x=position.x+size.x-vinv;
			
			int valueIndex=int(lastValues.size())-1-vinv;
			if(valueIndex<=1) break;
			
			float a=lastValues[valueIndex-1];
			float b=lastValues[valueIndex];
			
			screen.line(Pos(x-1,position.y+size.y-int(floor(a*size.y))),Pos(x,position.y+size.y-int(floor(b*size.y))),lineColor);
		}
	}
};

int getIndexOfMaximum(const vector<float>& v)
{
	if(v.size()==0) return 0;
	int index=0;
	float maximum=v[0];
	for(int i=1;i<v.size();i++)
	{
		if(v[i]>maximum)
		{
			maximum=v[i];
			index=i;
		}
	}
	return index;
}

int getIndexOfMinimum(const vector<float>& v)
{
	if(v.size()==0) return 0;
	int index=0;
	float minimum=v[0];
	for(int i=1;i<v.size();i++)
	{
		if(v[i]<minimum)
		{
			minimum=v[i];
			index=i;
		}
	}
	return index;
}

float getMaximum(const vector<float>& v,float maximum)
{
	int index=getIndexOfMaximum(v);
	if(v.size()>0)
	{
		if(v[index]>maximum) maximum=v[index];
	}
	return maximum;
}

float getMinimum(const vector<float>& v)
{
	float minimum=0;
	int index=getIndexOfMinimum(v);
	if(v.size()>0)
	{
		minimum=v[index];
	}
	return minimum;
}

vector<float> getVectorNormalized(const vector<float>& inputVector,float maximum)
{
	vector<float> v=inputVector;
	float factor=1;
	if(maximum!=0) factor=1/maximum;
	for(int i=0;i<v.size();i++)
	{
		v[i]*=factor;
	}
	return v;
}

string bitsPerSecondToString(float bps)
{
	if(bps>=1000000000000.0) return std::to_string(bps/1000000000000.0)+" Tb/s";
	else if(bps>=1000000000.0) return std::to_string(bps/1000000000.0)+" Gb/s";
	else if(bps>=1000000.0) return std::to_string(bps/1000000.0)+" Mb/s";
	else if(bps>=1000.0) return std::to_string(bps/1000.0)+" Kb/s";
	else return std::to_string(bps)+" b/s";
}

#include "computer.cpp"
#include "computer_builder.cpp"

template <class T>
void drawComputerState(graphics::Image& screen,const Computer::State<T>& computerState,const Pos& position,const Pos& size,int widthInPixels,int memoryOffset=0)
{
	int pixelSize=size.x/widthInPixels;
	
	screen.rect(position+Pos(-2,-2),position+size+Pos(1,1),0xffffff);
	for(int i=0;i<(size.x/pixelSize)*(size.y/pixelSize) && i<computerState.memory.size();i++)
	{
		Pos p=position+Pos(i%(size.x/pixelSize),i/(size.x/pixelSize))*pixelSize;
		int bit=computerState.getBitValue(computerState.memory[memoryOffset+i],0);
		
		int color= bit ? 0xffffff : 0;
		
		screen.rectfill(p,p+Pos(pixelSize,pixelSize)+Pos(-1,-1),color);
		screen.rect(p,p+Pos(pixelSize,pixelSize)+Pos(-1,-1),0x808080);
	}
}

bool stringStartsWith(const string& str,const string& start)
{
	if(str.size()<start.size()) return false;
	return str.substr(0,start.size())==start;
}

bool removeStringStart(string& str,const string& start)
{
	if(!stringStartsWith(str,start)) return false;
	str=str.substr(start.size(),str.size()-start.size());
	return true;
}

string translatePath(const std::filesystem::path& pathOfPath,const string& path)
{
	return (pathOfPath/path).string();
}

int main(int argc,char*argv[])
{
	try
	{
		if(argc<1)
		{
			throw string("Cannot get the path of the executable");
		}
		
		std::filesystem::path pathCalledFrom=std::filesystem::current_path();
		
		string argv0=string(argv[0]);
		
		if(argv0.find_first_of("/\\")!=string::npos)
		{
			std::filesystem::path path=argv0;
			path.remove_filename();
			std::filesystem::current_path(path);
		}
		
		vector<string> args;
		for(int i=1;i<argc;i++)
		{
			args.push_back(argv[i]);
		}
		
		if(args.size()==0)
		{
			throw string()+"Expected command";
		}
		string command=args[0];
		args.erase(args.begin());
		
		if(command=="optsearch")
		{
			string inputPath;
			ComputerBuilder::OptimizationSearchOptions options;
			string outputPath;
			
			
			for(int i=0;i<args.size();i++)
			{
				string arg=args[i];
				if(arg.size()>0)
				{
					if(arg[0]=='-')
					{
						if(arg.size()==1)
						{
							throw string()+"Expected option after the '-' character";
						}
						
						string argfull=arg;
						
						if(arg=="-s")
						{
							options.silent=true;
						}
						else if(removeStringStart(arg,"-alg="))
						{
							options.algorithm=arg;
						}
						else if(removeStringStart(arg,"-maxg="))
						{
							try
							{
								options.maxGates=std::stoi(arg);
							}
							catch(...)
							{
								throw string()+"Invalid value for option: '"+argfull+"'";
							}
						}
						else if(removeStringStart(arg,"-memory="))
						{
							uint64_t unit=(uint64_t(1)<<20);
							if(arg.size()>0)
							{
								if(arg[arg.size()-1]=='M')
								{
									unit=(uint64_t(1)<<20);
									arg.resize(arg.size()-1);
								}
								else if(arg[arg.size()-1]=='G')
								{
									unit=(uint64_t(1)<<30);
									arg.resize(arg.size()-1);
								}
							}
							
							try
							{
								int memory=std::stoi(arg);
								if(memory<=0) throw 1;
								options.memory=uint64_t(memory)*unit;
							}
							catch(...)
							{
								throw string()+"Invalid value for option: '"+argfull+"'";
							}
						}
						else if(removeStringStart(arg,"-maxt="))
						{
							try
							{
								options.maxTime=std::stod(arg);
								if(options.maxTime<0 && options.maxTime!=-1) throw 1;
							}
							catch(...)
							{
								throw string()+"Invalid value for option: '"+argfull+"'";
							}
						}
						else if(removeStringStart(arg,"-o="))
						{
							if(arg.size()==0)
							{
								throw string()+"Invalid value for option: '"+argfull+"'";
							}
							outputPath=arg;
						}
						else
						{
							throw string()+"'"+arg+"' option not recognized";
						}
					}
					else
					{
						inputPath=arg;
					}
				}
			}
			if(inputPath.size()==0)
			{
				throw string()+"Input path not specified";
			}
			if(outputPath.size()==0)
			{
				throw string()+"Output path not specified";
			}
			
			string inputCode;
			try
			{
				inputCode=fileToString(translatePath(pathCalledFrom,inputPath));
			}
			catch(...)
			{
				throw string()+"Could not load the file '"+inputPath+"'";
			}
			
			string outputCode=ComputerBuilder::optimizationSearch(inputCode,options);
			
			try
			{
				stringToFile(outputCode,outputPath);
			}
			catch(...)
			{
				throw string()+"Could not save the output";
			}
		}
		else if(command=="simulate")
		{
			string computerDescriptionCodePath;
			ComputerBuilder::OptimizationOptions optimizationOptions;
			string optimizationRulesCodePath;
			
			for(int i=0;i<args.size();i++)
			{
				string arg=args[i];
				if(arg.size()>0)
				{
					if(arg[0]=='-')
					{
						if(arg.size()==1)
						{
							throw string()+"Expected option after the '-' character";
						}
						
						string argfull=arg;
						
						if(arg=="-Og")
						{
							optimizationOptions.optimizeGates=true;
						}
						else if(arg=="-Om")
						{
							optimizationOptions.optimizeMemory=true;
						}
						else if(arg=="-Ov")
						{
							optimizationOptions.verbose=true;
						}
						else if(arg=="-Os")
						{
							optimizationOptions.silent=true;
						}
						else if(arg=="-Ow")
						{
							optimizationOptions.failAtWarning=true;
						}
						else if(removeStringStart(arg,"-Op="))
						{
							try
							{
								optimizationOptions.passes=std::stoi(arg);
								if(optimizationOptions.passes<-1) throw 1;
							}
							catch(...)
							{
								throw string()+"Invalid value for option: '"+argfull+"'";
							}
						}
						else if(removeStringStart(arg,"-Oc="))
						{
							try
							{
								optimizationOptions.maxCombinations=std::stoi(arg);
								if(optimizationOptions.maxCombinations<1) throw 1;
							}
							catch(...)
							{
								throw string()+"Invalid value for option: '"+argfull+"'";
							}
						}
						else if(removeStringStart(arg,"-Ot="))
						{
							try
							{
								optimizationOptions.maxTime=std::stod(arg);
								if(optimizationOptions.maxTime<0 && optimizationOptions.maxTime!=-1) throw 1;
							}
							catch(...)
							{
								throw string()+"Invalid value for option: '"+argfull+"'";
							}
						}
						else if(removeStringStart(arg,"-Of="))
						{
							if(arg.size()==0)
							{
								throw string()+"Invalid value for option: '"+argfull+"'";
							}
							optimizationRulesCodePath=arg;
						}
						else
						{
							throw string()+"'"+arg+"' option not recognized";
						}
					}
					else
					{
						computerDescriptionCodePath=arg;
					}
				}
			}
			if(computerDescriptionCodePath.size()==0)
			{
				throw string()+"Input path not specified";
			}
			
			string computerDescriptionCode;
			try
			{
				computerDescriptionCode=fileToString(translatePath(pathCalledFrom,computerDescriptionCodePath));
			}
			catch(...)
			{
				throw string()+"Could not load the file '"+computerDescriptionCodePath+"'";
			}
			
			string optimizationRulesCode;
			if(optimizationRulesCodePath.size()>0)
			{
				try
				{
					optimizationRulesCode=fileToString(translatePath(pathCalledFrom,optimizationRulesCodePath));
				}
				catch(...)
				{
					throw string()+"Could not load the file '"+optimizationRulesCodePath+"'";
				}
			}
			
			if(optimizationRulesCodePath.size()>0 && !optimizationOptions.optimizeGates)
			{
				throw string()+"Provided optimization rules, but those will not actually be used because the gate optimization option has not been set";
			}
			
			ComputerBuilder computerBuilder;
			Computer computer=computerBuilder.buildComputer(computerDescriptionCode,optimizationOptions,optimizationRulesCode);
			
			Computer::State<uint8_t> computerState=computer.getInitialState<Computer::State<uint8_t>>();
			
			
			int mov=16;
			int jnls=23;
			int mul=10;
			int add=8;
			int jls=22;
			int out=24;
			int j=17;
			
			int w=128;
			int r=64;
			int m=32;
			
			#define W(a) w,int(uint32_t(a)&0xff),int((uint32_t(a)>>8)&0xff)
			
			int loop1=16+3*3;
			int loop1_end=loop1+28+3*1;
			int loop2=loop1_end+6+3*1;
			int loop2_end=loop2+17+3*1;
			int loop3=loop2_end;
			vector<int> machineCode=vector<int>{
				mov,r|0,W(20),
				mov,r|1,W(-20*2),
				mov,r|2,0,
				mov,r|3,1,
				
				
				mov,r|4,0,
				jnls,W(loop1_end),r|4,r|0,
				//.loop1:
				
				mul,r|5,r|4,2,
				add,r|6,r|1,r|5,
				mov,m|r|6,r|3,
				mov,r|7,r|3,
				add,r|3,r|3,r|2,
				mov,r|2,r|7,
				
				add,r|4,r|4,1,
				jls,W(loop1),r|4,r|0,
				//.loop1_end:
				
				
				mov,r|4,0,
				jnls,W(loop2_end),r|4,r|0,
				//.loop2:
				
				mul,r|5,r|4,2,
				add,r|6,r|1,r|5,
				out,m|r|6,
				
				add,r|4,r|4,1,
				jls,W(loop2),r|4,r|0,
				//.loop2_end:
				
				
				//.loop3:
				j,W(loop3)
			};
			//Code in assembly:
			/*
			mov r0,20
			mov r1,-20*2
			mov r2,0
			mov r3,1
			
			
			mov r4,0
			jnls .loop1_end r4,r0
			.loop1:
			
			mul r5,r4,2
			add r6,r1,r5
			mov [r6],r3
			mov r7,r3
			add r3,r3,r2
			mov r2,r7
			
			add r4,r4,1
			jls .loop1 r4,r0
			.loop1_end:
			
			
			mov r4,0
			jnls .loop2_end r4,r0
			.loop2:
			
			mul r5,r4,2
			add r6,r1,r5
			out [r6]
			
			add r4,r4,1
			jls .loop2 r4,r0
			.loop2_end:
			
			
			.loop3:
			j .loop3
			*/
			
			//Code in C:
			/*
			int numbersToOutput=20;
			int output[20];
			int oldValue=0;
			int value=1;
			for(int i=0;i<numbersToOutput;i++)
			{
				output[i]=value;
				int newOldValue=value;
				value+=oldValue;
				oldValue=newOldValue;
			}
			for(int i=0;i<numbersToOutput;i++)
			{
				out(output[i]);
			}
			for(;;){}
			*/
			
			
			
			wrapper::Window window;
			window.create("Program");
			graphics::Image windowIcon("application/icon.bmp");
			window.setWindowIcon(windowIcon);
			
			graphics::Image textFont("textfont.bmp");
			
			uint64_t cycle=0;
			string output;
			
			bool play=false;
			
			while(window.pollEventsAndUpdateInputAndTime())
			{
				if(window.input.key[wrapper::key::SPACE] && !window.input.keyOld[wrapper::key::SPACE])
				{
					play=!play;
				}
				
				if(play || window.input.key['T'] && !window.input.keyOld['T'])
				{
					double timePerFrame=0.02;
					
					TimeCounter timeCounter;
					timeCounter.start();
					for(;;)
					{
						if(cycle<machineCode.size())
						{
							if(computerState.inputs.size()>=8)
							{
								uint8_t inputNumber=machineCode[cycle];
								for(int i=0;i<8;i++)
								{
									computerState.inputs[i]=computerState.getConstant((uint64_t(inputNumber)>>i)&1);
								}
							}
						}
						else if(cycle==machineCode.size())
						{
							if(computerState.inputs.size()>=9)
							{
								computerState.inputs[8]=computerState.getConstant(1);
							}
						}
						
						computerState=computer.simulateStep(computerState);
						
						if(computerState.outputs.size()>=17)
						{
							if(computerState.getBitValue(computerState.outputs[16],0))
							{
								uint64_t number=0;
								for(int i=0;i<16;i++)
								{
									number|=(uint64_t(computerState.getBitValue(computerState.outputs[i],0))<<i);
								}
								if(output.size()>0) output+=",";
								output+=std::to_string(number);
							}
						}
						
						cycle++;
						
						if(play)
						{
							if(timeCounter.end()>=timePerFrame) break;
						}
						else break;
					}
				}
				
				
				
				window.screen.clear(0x000000);
				
				window.screen.textprint(textFont,Pos(10,100),0xffffff,string("number of nand gates: ")+std::to_string(computer.nandGates.size()));
				window.screen.textprint(textFont,Pos(10,150),0xffffff,string("number of memory bits: ")+std::to_string(computer.memory.size()));
				
				window.screen.textprint(textFont,Pos(10,200),0xffffff,string("cycle: ")+std::to_string(cycle));
				
				for(int lineIndex=0;;lineIndex++)
				{
					int lineWidth=64;
					
					int from=lineIndex*lineWidth;
					int to_=(lineIndex+1)*lineWidth;
					
					if(from>=output.size()) break;
					
					string line;
					if(to_>output.size())
					{
						to_=output.size();
					}
					
					line=output.substr(from,to_-from);
					
					int lineY=lineIndex*(16+8);
					
					window.screen.textprint(textFont,Pos(10,600+lineY),0xffffff,line);
				}
				
				drawComputerState(window.screen,computerState,Pos(500,50),Pos(512,512),16);
				drawComputerState(window.screen,computerState,Pos(1100,50),Pos(512,512),64);
				
				window.drawScreen();
			}
			
			window.destroy();
		}
		else
		{
			throw string()+"Command '"+command+"' not recognized";
		}
	}
	catch(const string& str)
	{
		std::cout<<str<<std::endl;
		return 1;
	}
	
	return 0;
}

