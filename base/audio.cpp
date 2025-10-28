namespace audio
{
	const static int sampleRate=48000;
	
	typedef struct __attribute__((packed))
	{
		int32_t ChunkID;
		int32_t ChunkSize;
		int32_t format;
		int32_t Subchunk1ID;
		int32_t Subchunk1Size;
		int16_t AudioFormat;
		int16_t NumChannels;
		int32_t SampleRate;
		int32_t ByteRate;
		int16_t BlockAlign;
		int16_t BitsPerSample;
	}WAV_file_header;
	
	class Audio
	{
		public:
		vector<float> channels[2];
		float duration()
		{
			return float(channels[0].size())/sampleRate;
		}
		Audio(){}
		explicit Audio(const string& filepath)
		{
			if(!load(filepath)) throw string("Could not load audio file");
		}
		explicit Audio(const vector<float>& mono)
		{
			channels[0]=mono;
			channels[1]=mono;
		}
		vector<float> getMono()
		{
			vector<float> mono(channels[0].size());
			for(size_t n=0;n<mono.size();n++)
			{
				mono[n]=(channels[0][n]+channels[1][n])/2;
			}
			return mono;
		}
		bool save(const string& filepath)
		{
			if(channels[0].size()!=channels[1].size()) return false;
			
			uint64_t size_of_data=2*2*channels[0].size();
			vector<uint8_t> data(size_of_data);
			
			for(unsigned int n=0;n<channels[0].size();n++)
			{
				*(int16_t*)&data[n*4]=channels[0][n]>1 ? 1 : (channels[0][n]<-1 ? -1 : int16_t(channels[0][n]*0x7fff));
				*(int16_t*)&data[n*4+2]=channels[1][n]>1 ? 1 : (channels[1][n]<-1 ? -1 : int16_t(channels[1][n]*0x7fff));
			}
			
			WAV_file_header wav_file_header;
			wav_file_header.ChunkID=0x46464952;
			if((size_of_data+36)>>32) return false;
			wav_file_header.ChunkSize=size_of_data+36;
			wav_file_header.format=0x45564157;
			wav_file_header.Subchunk1ID=0x20746d66;
			wav_file_header.Subchunk1Size=16;
			wav_file_header.AudioFormat=1;
			wav_file_header.NumChannels=2;
			wav_file_header.SampleRate=sampleRate;
			wav_file_header.ByteRate=sampleRate*2*2;
			wav_file_header.BlockAlign=2*2;
			wav_file_header.BitsPerSample=16;
			
			std::ofstream file;
			file.open(filepath.c_str(),std::ios::binary);
			if(!file.is_open()) return false;
			
			file.write((char*)&wav_file_header,sizeof(WAV_file_header));
			
			file.write("data",4);
			
			file.write((char*)&size_of_data,4);
			
			file.write((char*)data.data(),data.size());
			
			file.close();
			
			return true;
		}
		bool load(const string& filepath)
		{
			std::ifstream file;
			file.open(filepath.c_str(),std::ios::binary);
			if(!file.good()) return false;
			
			WAV_file_header wav_file_header;
			file.read((char*)&wav_file_header,sizeof(WAV_file_header));
			
			if(wav_file_header.ChunkID!=0x46464952) return false;
			if(wav_file_header.format!=0x45564157) return false;
			if(wav_file_header.Subchunk1ID!=0x20746d66) return false;
			if(wav_file_header.Subchunk1Size!=16) return false;
			if(wav_file_header.AudioFormat!=1) return false;
			
			int num_channels=wav_file_header.NumChannels;
			
			if(wav_file_header.SampleRate!=sampleRate) return false;
			int bytes_per_sample=wav_file_header.BitsPerSample/8;
			if(bytes_per_sample!=2) return false;
			
			char str[4]={0,0,0,0};
			for(int n=0;;n++)
			{
				if(n==1000) return false;
				char c;
				file.read(&c,1);
				for(int i=0;i<3;i++)
				{
					str[i]=str[i+1];
				}
				str[3]=c;
				if(str[0]=='d' && str[1]=='a' && str[2]=='t' && str[3]=='a') break;
			}
			size_t size_of_data=0;
			file.read((char*)&size_of_data,4);
			
			vector<uint8_t> data(size_of_data);
			file.read((char*)data.data(),data.size());
			
			file.close();
			
			if(num_channels==1)
			{
				channels[0].resize(data.size()/2);
				channels[1].resize(data.size()/2);
				for(size_t n=0;n<data.size();n+=2)
				{
					channels[0][n/2]=float(int(*((short int*)&data[n])))/0x8000;
					channels[1][n/2]=channels[0][n/2];
				}
			}
			else
			{
				channels[0].resize(data.size()/4);
				channels[1].resize(data.size()/4);
				for(size_t n=0;n<data.size();n+=4)
				{
					channels[0][n/4]=float(int(*((short int*)&data[n])))/0x8000;
					channels[1][n/4]=float(int(*((short int*)&data[n+2])))/0x8000;
				}
			}
			
			return true;
		}
	};
}
