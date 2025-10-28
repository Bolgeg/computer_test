namespace wrapper
{
	namespace key
	{
		const static int F1=128+1;
		const static int F2=128+2;
		const static int F3=128+3;
		const static int F4=128+4;
		const static int F5=128+5;
		const static int F6=128+6;
		const static int F7=128+7;
		const static int F8=128+8;
		const static int F9=128+9;
		const static int F10=128+10;
		const static int F11=128+11;
		const static int F12=128+12;
		
		const static int BACKSPACE=160;
		const static int CAPSLOCK=161;
		const static int DELETE=162;
		
		const static int UP=163;
		const static int DOWN=164;
		const static int LEFT=165;
		const static int RIGHT=166;
		
		const static int ESCAPE=167;
		
		const static int LALT=168;
		const static int RALT=169;
		const static int LCTRL=170;
		const static int RCTRL=171;
		const static int LSHIFT=172;
		const static int RSHIFT=173;
		
		const static int ENTER=174;
		const static int PERIOD=175;
		const static int SPACE=176;
		const static int TAB=177;
		
		const static int LBUTTON=200;
		const static int MBUTTON=201;
		const static int RBUTTON=202;
	}
	
	class Input
	{
		public:
		vector<bool> key=vector<bool>(256,false);
		vector<bool> keyOld=vector<bool>(256,false);
		Pos mouse=Pos(0,0);
		Pos mouseOld=Pos(0,0);
		int mouseWheel=0;
	};
	
	void (*CURRENT_AUDIO_CALLBACK_FUNCTION)(float*,int)=nullptr;
	
	void audioCallbackWrapper(void*userdata,Uint8*stream,int len)
	{
		if(CURRENT_AUDIO_CALLBACK_FUNCTION!=nullptr) (*CURRENT_AUDIO_CALLBACK_FUNCTION)((float*)stream,len/8);
	}
	
	void (*CURRENT_INPUT_AUDIO_CALLBACK_FUNCTION)(float*,int)=nullptr;
	
	void inputAudioCallbackWrapper(void*userdata,Uint8*stream,int len)
	{
		if(CURRENT_INPUT_AUDIO_CALLBACK_FUNCTION!=nullptr) (*CURRENT_INPUT_AUDIO_CALLBACK_FUNCTION)((float*)stream,len/4);
	}
	
	class Window
	{
		public:
		bool created=false;
		Pos minimumResolution=Pos(0,0);
		Pos maximumResolution=Pos(0,0);
		SDL_Window*window=nullptr;
		SDL_Renderer*renderer=nullptr;
		SDL_Texture*screenTexture=nullptr;
		graphics::Image screen;
		Input input;
		std::map<SDL_Keycode,int> keyMap;
		unsigned int lastFrameTime=0;
		double SF=0;
		bool audioInitialized=false;
		bool audioDeviceOpened=false;
		SDL_AudioDeviceID audioDeviceId;
		bool inputAudioDeviceOpened=false;
		SDL_AudioDeviceID inputAudioDeviceId;
		
		bool textInputCallbackFunctionsSet=false;
		std::function<void(string)> textInputCallbackFunction;
		std::function<void(uint8_t)> textInputSpecialCallbackFunction;
		std::function<void(string,size_t,size_t)> textInputEditingCallbackFunction;
		bool textInputIsAlive=false;
		bool textInputIsOpen=false;
		
		void create(const string& windowTitle="Program",Pos _minimumResolution=Pos(0,0))
		{
			if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)!=0)
			{
				throw string("SDL could not be initialized");
			}
			
			Pos windowResolution=Pos(640,480);
			if(minimumResolution.x>windowResolution.x || minimumResolution.y>windowResolution.y)
			{
				windowResolution=minimumResolution;
			}
			
			window=SDL_CreateWindow(windowTitle.c_str(),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,windowResolution.x,windowResolution.y,
				SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_OPENGL);
			if(window==nullptr)
			{
				throw string("The window could not be created");
			}
			renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_PRESENTVSYNC);
			if(renderer==nullptr)
			{
				throw string("The window renderer could not be created");
			}
			
			minimumResolution=_minimumResolution;
			if(minimumResolution!=Pos(0,0))
			{
				SDL_SetWindowMinimumSize(window,minimumResolution.x,minimumResolution.y);
			}
			
			maximumResolution=getScreenResolution();
			
			screenTexture=SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,maximumResolution.x,maximumResolution.y);
			
			screen=graphics::Image(maximumResolution);
			
			initializeKeyMap();
			
			created=true;
		}
		void destroy()
		{
			closeAudio();
			
			SDL_Quit();
		}
		
		private:
			void setTextInputCallbackFunctions(std::function<void(string)> _textInputCallbackFunction,
				std::function<void(uint8_t)> _textInputSpecialCallbackFunction,
					std::function<void(string,size_t,size_t)> _textInputEditingCallbackFunction)
			{
				textInputCallbackFunctionsSet=true;
				textInputCallbackFunction=_textInputCallbackFunction;
				textInputSpecialCallbackFunction=_textInputSpecialCallbackFunction;
				textInputEditingCallbackFunction=_textInputEditingCallbackFunction;
			}
			void disableTextInputCallbackFunctions()
			{
				textInputCallbackFunctionsSet=false;
			}
			void stopTextInput()
			{
				SDL_StopTextInput();
				
				disableTextInputCallbackFunctions();
			}
		public:
		void startTextInput(const Pos& position,const Pos& size,
			std::function<void(string)> _textInputCallbackFunction,
				std::function<void(uint8_t)> _textInputSpecialCallbackFunction,
					std::function<void(string,size_t,size_t)> _textInputEditingCallbackFunction)
		{
			if(textInputCallbackFunctionsSet)
			{
				stopTextInput();
			}
			
			setTextInputCallbackFunctions(
				_textInputCallbackFunction,
				_textInputSpecialCallbackFunction,
				_textInputEditingCallbackFunction
			);
			
			SDL_Rect rect;
			rect.x=position.x;
			rect.y=position.y;
			rect.w=size.x;
			rect.h=size.y;
			SDL_SetTextInputRect(&rect);
			SDL_StartTextInput();
			
			textInputIsAlive=true;
		}
		void setTextInputRectangle(const Pos& position,const Pos& size)
		{
			if(textInputCallbackFunctionsSet)
			{
				SDL_Rect rect;
				rect.x=position.x;
				rect.y=position.y;
				rect.w=size.x;
				rect.h=size.y;
				SDL_SetTextInputRect(&rect);
			}
		}
		void textInputKeepAlive()
		{
			textInputIsAlive=true;
		}
		bool inputtingText()
		{
			return textInputIsOpen;
		}
		
		void setWindowIcon(graphics::Image& image)
		{
			SDL_Surface*icon=SDL_CreateRGBSurfaceFrom(image.data(),image.size().x,image.size().y,32,4*image.size().x,0xff0000,0x00ff00,0x0000ff,0);
			SDL_SetWindowIcon(window,icon);
		}
		void setFullscreen(bool fullscreen)
		{
			SDL_SetWindowFullscreen(window,fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
		}
		string getClipboardText()
		{
			char*s=SDL_GetClipboardText();
			string str(s);
			SDL_free(s);
			return str;
		}
		void setClipboardText(const string& str)
		{
			SDL_SetClipboardText(str.c_str());
		}
		Pos getScreenResolution()
		{
			SDL_DisplayMode dm;
			if(SDL_GetDesktopDisplayMode(0,&dm))
			{
				throw string("Could not get the size of the display");
			}
			return Pos(dm.w,dm.h);
		}
		Pos getWindowSize()
		{
			Pos p;
			SDL_GetWindowSize(window,&p.x,&p.y);
			return p;
		}
		unsigned int getMilliseconds()
		{
			return SDL_GetTicks();
		}
		void drawScreen()
		{
			SDL_UpdateTexture(screenTexture,NULL,screen.data(),screen.size().x*sizeof(int));
			SDL_RenderClear(renderer);
			
			Pos s=getWindowSize();
			SDL_Rect rect;
			rect.x=0;
			rect.y=0;
			rect.w=s.x;
			rect.h=s.y;
			
			SDL_RenderCopy(renderer,screenTexture,&rect,&rect);
			SDL_RenderPresent(renderer);
		}
		bool pollEvents()
		{
			bool continueRunning=true;
			SDL_Event event;
			
			input.mouseWheel=0;
			
			textInputIsOpen=textInputIsAlive;
			if(!textInputIsAlive) stopTextInput();
			textInputIsAlive=false;
			
			while(SDL_PollEvent(&event))
			{
				if(event.type==SDL_QUIT)
				{
					continueRunning=false;
					break;
				}
				else if(event.type==SDL_MOUSEWHEEL)
				{
					input.mouseWheel-=event.wheel.y;
				}
				else if(event.type==SDL_TEXTINPUT)
				{
					if(textInputCallbackFunctionsSet)
					{
						textInputCallbackFunction(string(event.text.text));
					}
				}
				else if(event.type==SDL_TEXTEDITING)
				{
					if(textInputCallbackFunctionsSet)
					{
						textInputEditingCallbackFunction(string(event.edit.text),event.edit.start,event.edit.length);
					}
				}
				else if(event.type==SDL_KEYDOWN)
				{
					if(textInputCallbackFunctionsSet)
					{
						int key=event.key.keysym.sym;
						if(key=='\r')
						{
							key='\n';
						}
						if(key=='\n' || key=='\t' || key=='\b')
						{
							textInputSpecialCallbackFunction(key);
						}
					}
				}
			}
			return continueRunning;
		}
		void updateTime()
		{
			unsigned int timeNow=getMilliseconds();
			unsigned int time=timeNow-lastFrameTime;
			lastFrameTime=timeNow;
			SF=double(time)/1000;
		}
		bool pollEventsAndUpdateInputAndTime()
		{
			bool continueRunning=pollEvents();
			if(!continueRunning) return false;
			updateInput();
			updateTime();
			return continueRunning;
		}
		void initializeKeyMap()
		{
			keyMap[SDLK_1]='1';
			keyMap[SDLK_2]='2';
			keyMap[SDLK_3]='3';
			keyMap[SDLK_4]='4';
			keyMap[SDLK_5]='5';
			keyMap[SDLK_6]='6';
			keyMap[SDLK_7]='7';
			keyMap[SDLK_8]='8';
			keyMap[SDLK_9]='9';
			keyMap[SDLK_0]='0';
			
			keyMap[SDLK_q]='Q';
			keyMap[SDLK_w]='W';
			keyMap[SDLK_e]='E';
			keyMap[SDLK_r]='R';
			keyMap[SDLK_t]='T';
			keyMap[SDLK_y]='Y';
			keyMap[SDLK_u]='U';
			keyMap[SDLK_i]='I';
			keyMap[SDLK_o]='O';
			keyMap[SDLK_p]='P';
			keyMap[SDLK_a]='A';
			keyMap[SDLK_s]='S';
			keyMap[SDLK_d]='D';
			keyMap[SDLK_f]='F';
			keyMap[SDLK_g]='G';
			keyMap[SDLK_h]='H';
			keyMap[SDLK_j]='J';
			keyMap[SDLK_k]='K';
			keyMap[SDLK_l]='L';
			keyMap[SDLK_z]='Z';
			keyMap[SDLK_x]='X';
			keyMap[SDLK_c]='C';
			keyMap[SDLK_v]='V';
			keyMap[SDLK_b]='B';
			keyMap[SDLK_n]='N';
			keyMap[SDLK_m]='M';
			
			keyMap[SDLK_F1]=key::F1;
			keyMap[SDLK_F2]=key::F2;
			keyMap[SDLK_F3]=key::F3;
			keyMap[SDLK_F4]=key::F4;
			keyMap[SDLK_F5]=key::F5;
			keyMap[SDLK_F6]=key::F6;
			keyMap[SDLK_F7]=key::F7;
			keyMap[SDLK_F8]=key::F8;
			keyMap[SDLK_F9]=key::F9;
			keyMap[SDLK_F10]=key::F10;
			keyMap[SDLK_F11]=key::F11;
			keyMap[SDLK_F12]=key::F12;
			
			keyMap[SDLK_BACKSPACE]=key::BACKSPACE;
			keyMap[SDLK_CAPSLOCK]=key::CAPSLOCK;
			keyMap[SDLK_DELETE]=key::DELETE;
			
			keyMap[SDLK_UP]=key::UP;
			keyMap[SDLK_DOWN]=key::DOWN;
			keyMap[SDLK_LEFT]=key::LEFT;
			keyMap[SDLK_RIGHT]=key::RIGHT;
			
			keyMap[SDLK_ESCAPE]=key::ESCAPE;
			
			keyMap[SDLK_LALT]=key::LALT;
			keyMap[SDLK_RALT]=key::RALT;
			keyMap[SDLK_LCTRL]=key::LCTRL;
			keyMap[SDLK_RCTRL]=key::RCTRL;
			keyMap[SDLK_LSHIFT]=key::LSHIFT;
			keyMap[SDLK_RSHIFT]=key::RSHIFT;
			
			keyMap[SDLK_RETURN]=key::ENTER;
			keyMap[SDLK_PERIOD]=key::PERIOD;
			keyMap[SDLK_SPACE]=key::SPACE;
			keyMap[SDLK_TAB]=key::TAB;
		}
		void updateInput()
		{
			input.keyOld=input.key;
			for(int k=0;k<input.key.size();k++)
			{
				input.key[k]=false;
			}
			int numKeys;
			const Uint8*keys=SDL_GetKeyboardState(&numKeys);
			for(int k=0;k<numKeys;k++)
			{
				if(keys[k])
				{
					SDL_Keycode keycode=SDL_GetKeyFromScancode((SDL_Scancode)k);
					try
					{
						int keyNumber=keyMap.at(keycode);
						if(keyNumber>=0 && keyNumber<input.key.size()) input.key[keyNumber]=true;
					}
					catch(...){}
				}
			}
			input.mouseOld=input.mouse;
			Uint32 buttons=SDL_GetMouseState(&input.mouse.x,&input.mouse.y);
			if(buttons&SDL_BUTTON_LMASK) input.key[key::LBUTTON]=true;
			if(buttons&SDL_BUTTON_MMASK) input.key[key::MBUTTON]=true;
			if(buttons&SDL_BUTTON_RMASK) input.key[key::RBUTTON]=true;
		}
		void setMousePosition(const Pos& position)
		{
			SDL_WarpMouseInWindow(window,position.x,position.y);
		}
		void setShowCursor(bool show)
		{
			if(show) SDL_ShowCursor(SDL_ENABLE);
			else SDL_ShowCursor(SDL_DISABLE);
		}
		void initializeAudio()
		{
			SDL_Init(SDL_INIT_AUDIO);
			audioInitialized=true;
		}
		void pauseAudio()
		{
			SDL_PauseAudioDevice(audioDeviceId,1);
		}
		void unpauseAudio()
		{
			SDL_PauseAudioDevice(audioDeviceId,0);
		}
		void openAudio(void (*callbackFunction)(float*,int),int samples)
		{
			if(!audioInitialized)
			{
				initializeAudio();
			}
			
			if(audioDeviceOpened) throw string("The audio device is already opened");
			
			SDL_AudioSpec want,have;
			SDL_memset(&want,0,sizeof(want));
			want.freq=audio::sampleRate;
			want.format=AUDIO_F32SYS;
			want.channels=2;
			want.samples=samples;
			want.callback=audioCallbackWrapper;
			CURRENT_AUDIO_CALLBACK_FUNCTION=callbackFunction;
			audioDeviceId=SDL_OpenAudioDevice(NULL,0,&want,&have,0);
			if(audioDeviceId==0)
			{
				throw string("Could not open the audio device: ")+SDL_GetError();
			}
			
			unpauseAudio();
			
			audioDeviceOpened=true;
		}
		void closeAudio()
		{
			if(audioDeviceOpened)
			{
				pauseAudio();
				SDL_CloseAudioDevice(audioDeviceId);
				CURRENT_AUDIO_CALLBACK_FUNCTION=nullptr;
				audioDeviceOpened=false;
			}
			if(inputAudioDeviceOpened)
			{
				pauseInputAudio();
				SDL_CloseAudioDevice(inputAudioDeviceId);
				CURRENT_INPUT_AUDIO_CALLBACK_FUNCTION=nullptr;
				inputAudioDeviceOpened=false;
			}
		}
		void pauseInputAudio()
		{
			SDL_PauseAudioDevice(inputAudioDeviceId,1);
		}
		void unpauseInputAudio()
		{
			SDL_PauseAudioDevice(inputAudioDeviceId,0);
		}
		void openInputAudio(void (*callbackFunction)(float*,int),int samples)
		{
			if(!audioInitialized)
			{
				initializeAudio();
			}
			
			if(inputAudioDeviceOpened) throw string("The input audio device is already opened");
			
			SDL_AudioSpec want,have;
			SDL_memset(&want,0,sizeof(want));
			want.freq=audio::sampleRate;
			want.format=AUDIO_F32SYS;
			want.channels=1;
			want.samples=samples;
			want.callback=inputAudioCallbackWrapper;
			CURRENT_INPUT_AUDIO_CALLBACK_FUNCTION=callbackFunction;
			inputAudioDeviceId=SDL_OpenAudioDevice(NULL,1,&want,&have,0);
			if(inputAudioDeviceId==0)
			{
				throw string("Could not open the input audio device: ")+SDL_GetError();
			}
			
			unpauseInputAudio();
			
			inputAudioDeviceOpened=true;
		}
	};
}
