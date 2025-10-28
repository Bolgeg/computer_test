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

int main(int argc,char*argv[])
{
	try
	{
	if(argc<1)
	{
		throw string("Cannot get the path of the executable");
	}
	
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
	
	
	string computerDescriptionCode=fileToString("computer.txt");
	
	ComputerBuilder computerBuilder;
	Computer computer=computerBuilder.buildComputer(computerDescriptionCode);
	
	Computer::State computerState=computer.getInitialState();
	
	
	
	wrapper::Window window;
	window.create("Program");
	graphics::Image windowIcon("application/icon.bmp");
	window.setWindowIcon(windowIcon);
	
	graphics::Image textFont("textfont.bmp");
	
	uint64_t step=0;
	string output;
	
	while(window.pollEventsAndUpdateInputAndTime())
	{
		if(window.input.key['T'] && !window.input.keyOld['T'])
		{
			int inputNumber=1+step;
			for(int i=0;i<16 && i<computerState.inputs.size();i++)
			{
				computerState.inputs[i]=(uint64_t(inputNumber)>>i)&1;
			}
			
			computerState=computer.simulateStep(computerState);
			
			if(computerState.outputs.size()>=17)
			{
				if(computerState.outputs[16])
				{
					uint64_t number=0;
					for(int i=0;i<16;i++)
					{
						number|=(computerState.outputs[i]<<i);
					}
					if(output.size()>0) output+=",";
					output+=std::to_string(number);
				}
			}
			
			step++;
		}
		
		
		
		window.screen.clear(0x000000);
		
		window.screen.textprint(textFont,Pos(10,200),0xffffff,string("steps: ")+std::to_string(step));
		
		window.screen.textprint(textFont,Pos(10,300),0xffffff,output);
		
		
		
		
		window.drawScreen();
	}
	
	window.destroy();
	
	}catch(const string& str)
	{
		std::cout<<str<<std::endl;
	}
	
	return 0;
}

