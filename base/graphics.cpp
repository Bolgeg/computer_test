namespace graphics
{
	class DrawingArea
	{
		public:
		Pos min=Pos(0,0);
		Pos max=Pos(0,0);
		bool isDefault=true;
		DrawingArea(){}
		DrawingArea(const Pos& _min,const Pos& _max)
		{
			min=_min;
			max=_max;
			isDefault=false;
		}
		bool contains(const Pos& p) const
		{
			return p.within(min,max);
		}
		bool isEmpty() const
		{
			return min.x>max.x || min.y>max.y;
		}
		static void intersection1D(int amin,int amax,int bmin,int bmax,int& rmin,int& rmax)
		{
			if(amax>=bmin && bmax>=amin)
			{
				if(amin>=bmin)
				{
					if(bmax>=amax)
					{
						rmin=amin;
						rmax=amax;
					}
					else
					{
						rmin=amin;
						rmax=bmax;
					}
				}
				else
				{
					if(amax>=bmax)
					{
						rmin=bmin;
						rmax=bmax;
					}
					else
					{
						rmin=bmin;
						rmax=amax;
					}
				}
			}
			else
			{
				rmin=0;
				rmax=-1;
			}
		}
		static DrawingArea intersection(const DrawingArea& a,const DrawingArea& b)
		{
			DrawingArea area(Pos(0,0),Pos(0,0));
			intersection1D(a.min.x,a.max.x,b.min.x,b.max.x,area.min.x,area.max.x);
			intersection1D(a.min.y,a.max.y,b.min.y,b.max.y,area.min.y,area.max.y);
			return area;
		}
	}DEFAULT_DRAWING_AREA;

	class Image
	{
		private:
		Pos internalSize=Pos(0,0);
		vector<uint32_t> internalPixels;
		public:
		Pos size() const
		{
			return internalSize;
		}
		uint32_t*data()
		{
			return internalPixels.data();
		}
		Image(){}
		explicit Image(const Pos& _size)
		{
			internalSize=_size;
			if(internalSize.x<0) internalSize.x=0;
			if(internalSize.y<0) internalSize.y=0;
			internalPixels=vector<uint32_t>(internalSize.x*internalSize.y);
		}
		explicit Image(const string& filePath)
		{
			load(filePath);
		}
		void load(const string& filePath)
		{
			binary::Binary binary;
			binary.loadFromFile(filePath);
			
			binary.cursor=18;
			internalSize.x=binary.read<uint32_t>();
			internalSize.y=binary.read<uint32_t>();
			internalPixels=vector<uint32_t>(internalSize.x*internalSize.y);
			
			binary.cursor=54;
			size_t padding=(4-((internalSize.x*3)%4))%4;
			for(int y=internalSize.y-1;y>=0;y--)
			{
				for(int x=0;x<internalSize.x;x++)
				{
					uint32_t color=binary.read<uint8_t>();
					color|=binary.read<uint8_t>()<<8;
					color|=binary.read<uint8_t>()<<16;
					internalPixels[y*internalSize.x+x]=color;
				}
				binary.cursor+=padding;
			}
		}
		void save(const string& filePath)
		{
			binary::Binary binary;
			
			binary.write<uint8_t>('B');
			binary.write<uint8_t>('M');
			
			size_t sizePosition=binary.size();
			
			binary.write<uint32_t>(0);
			binary.write<uint32_t>(0);
			binary.write<uint32_t>(0x36);
			binary.write<uint32_t>(40);
			binary.write<uint32_t>(internalSize.x);
			binary.write<uint32_t>(internalSize.y);
			
			binary.write<uint16_t>(1);
			binary.write<uint16_t>(24);
			
			binary.write<uint32_t>(0);
			binary.write<uint32_t>(0);
			binary.write<uint32_t>(0);
			binary.write<uint32_t>(0);
			binary.write<uint32_t>(0);
			binary.write<uint32_t>(0);
			
			size_t padding=(4-((internalSize.x*3)%4))%4;
			for(int y=internalSize.y-1;y>=0;y--)
			{
				for(int x=0;x<internalSize.x;x++)
				{
					uint32_t color=internalPixels[y*internalSize.x+x];
					binary.write<uint8_t>(color&0xff);
					binary.write<uint8_t>((color>>8)&0xff);
					binary.write<uint8_t>((color>>16)&0xff);
				}
				for(size_t n=0;n<padding;n++)
				{
					binary.write<uint8_t>(0);
				}
			}
			
			binary.content[sizePosition]=binary.size()&0xff;
			binary.content[sizePosition+1]=(binary.size()>>8)&0xff;
			binary.content[sizePosition+2]=(binary.size()>>16)&0xff;
			binary.content[sizePosition+3]=(binary.size()>>24)&0xff;
			
			binary.saveToFile(filePath);
		}
		void clear(uint32_t color)
		{
			for(size_t p=0;p<internalPixels.size();p++)
			{
				internalPixels[p]=color;
			}
		}
		uint32_t getpixel(const Pos& p) const
		{
			if(uint32_t(p.x)>=uint32_t(internalSize.x) || uint32_t(p.y)>=uint32_t(internalSize.y)) return -1;
			else return internalPixels[p.y*internalSize.x+p.x];
		}
		uint32_t _getpixel(const Pos& p) const
		{
			return internalPixels[p.y*internalSize.x+p.x];
		}
		void putpixel(const Pos& p,uint32_t color)
		{
			if(uint32_t(p.x)<uint32_t(internalSize.x) && uint32_t(p.y)<uint32_t(internalSize.y))
			{
				internalPixels[p.y*internalSize.x+p.x]=color;
			}
		}
		void _putpixel(const Pos& p,uint32_t color)
		{
			internalPixels[p.y*internalSize.x+p.x]=color;
		}
		DrawingArea getDrawingArea() const
		{
			return DrawingArea(Pos(0,0),Pos(internalSize.x-1,internalSize.y-1));
		}
		void adjustDrawingArea(DrawingArea& drawingArea) const
		{
			DrawingArea full=getDrawingArea();
			if(drawingArea.isDefault) drawingArea=full;
			else drawingArea=DrawingArea::intersection(drawingArea,full);
		}
		void putpixel(const Pos& p,uint32_t color,DrawingArea drawingArea)
		{
			adjustDrawingArea(drawingArea);
			if(drawingArea.contains(p)) internalPixels[p.y*internalSize.x+p.x]=color;
		}
		void rectfill(const Pos& min,const Pos& max,uint32_t color,DrawingArea drawingArea=DEFAULT_DRAWING_AREA)
		{
			adjustDrawingArea(drawingArea);
			DrawingArea area=DrawingArea::intersection(drawingArea,DrawingArea(min,max));
			if(!area.isEmpty())
			{
				for(int y=area.min.y;y<=area.max.y;y++)
				{
					for(int x=area.min.x;x<=area.max.x;x++)
					{
						internalPixels[y*internalSize.x+x]=color;
					}
				}
			}
		}
		void hline(Pos p,int x2,uint32_t color,DrawingArea drawingArea=DEFAULT_DRAWING_AREA)
		{
			adjustDrawingArea(drawingArea);
			if(x2<p.x) std::swap(p.x,x2);
			DrawingArea area=DrawingArea::intersection(drawingArea,DrawingArea(p,Pos(x2,p.y)));
			if(!area.isEmpty())
			{
				int y=area.min.y;
				for(int x=area.min.x;x<=area.max.x;x++)
				{
					internalPixels[y*internalSize.x+x]=color;
				}
			}
		}
		void vline(Pos p,int y2,uint32_t color,DrawingArea drawingArea=DEFAULT_DRAWING_AREA)
		{
			adjustDrawingArea(drawingArea);
			if(y2<p.y) std::swap(p.y,y2);
			DrawingArea area=DrawingArea::intersection(drawingArea,DrawingArea(p,Pos(p.x,y2)));
			if(!area.isEmpty())
			{
				int x=area.min.x;
				for(int y=area.min.y;y<=area.max.y;y++)
				{
					internalPixels[y*internalSize.x+x]=color;
				}
			}
		}
		void line(const Pos& a,const Pos& b,uint32_t color,DrawingArea drawingArea=DEFAULT_DRAWING_AREA)
		{
			if(a.y==b.y) hline(a,b.x,color,drawingArea);
			else if(a.x==b.x) vline(a,b.y,color,drawingArea);
			else
			{
				adjustDrawingArea(drawingArea);
				int d=std::ceil((b-a).d<float>())+1;
				for(int n=0;n<=d;n++)
				{
					Pos position=Pos(std::lerp(a.x,b.x,n*(float(1)/d)),std::lerp(a.y,b.y,n*(float(1)/d)));
					if(drawingArea.contains(position)) internalPixels[position.y*internalSize.x+position.x]=color;
				}
			}
		}
		void rect(const Pos& min,const Pos& max,uint32_t color,DrawingArea drawingArea=DEFAULT_DRAWING_AREA)
		{
			hline(min,max.x,color,drawingArea);
			hline(Pos(min.x,max.y),max.x,color,drawingArea);
			vline(min,max.y,color,drawingArea);
			vline(Pos(max.x,min.y),max.y,color,drawingArea);
		}
		void circlefill(const Vector2& center,float radius,uint32_t color,DrawingArea drawingArea=DEFAULT_DRAWING_AREA)
		{
			adjustDrawingArea(drawingArea);
			Pos min=Vector2(center.x-radius,center.y-radius).floor();
			Pos max=Vector2(center.x+radius,center.y+radius).ceil()-Pos(1,1);
			DrawingArea area=DrawingArea::intersection(drawingArea,DrawingArea(min,max));
			if(!area.isEmpty())
			{
				float radius2=radius*radius;
				for(int y=area.min.y;y<=area.max.y;y++)
				{
					for(int x=area.min.x;x<=area.max.x;x++)
					{
						if(Vector2Type<float>(center.x-x,center.y-y).d2()<=radius2) internalPixels[y*internalSize.x+x]=color;
					}
				}
			}
		}
		DrawingArea blitDrawingArea(const Image& image,const Pos& destinationPosition,const Pos& sourcePosition,const Pos& blitSize,DrawingArea drawingArea=DEFAULT_DRAWING_AREA) const
		{
			adjustDrawingArea(drawingArea);
			DrawingArea areaDestination=DrawingArea::intersection(drawingArea,DrawingArea(destinationPosition,destinationPosition+blitSize-Pos(1,1)));
			DrawingArea areaSource=DrawingArea::intersection(image.getDrawingArea(),DrawingArea(sourcePosition,sourcePosition+blitSize-Pos(1,1)));
			DrawingArea finalArea=DrawingArea::intersection(areaDestination,DrawingArea(areaSource.min+destinationPosition-sourcePosition,areaSource.max+destinationPosition-sourcePosition));
			return finalArea;
		}
		void blit(const Image& image,const Pos& destinationPosition=Pos(0,0),const Pos& sourcePosition=Pos(0,0),Pos blitSize=Pos(-1,-1),DrawingArea drawingArea=DEFAULT_DRAWING_AREA)
		{
			if(blitSize==Pos(-1,-1)) blitSize=image.size();
			DrawingArea area=blitDrawingArea(image,destinationPosition,sourcePosition,blitSize,drawingArea);
			if(!area.isEmpty())
			{
				for(int y=area.min.y;y<=area.max.y;y++)
				{
					for(int x=area.min.x;x<=area.max.x;x++)
					{
						internalPixels[y*internalSize.x+x]=image._getpixel(Pos(x,y)-destinationPosition+sourcePosition);
					}
				}
			}
		}
		void maskedBlit(const Image& image,uint32_t maskColor,const Pos& destinationPosition,const Pos& sourcePosition,const Pos& blitSize,DrawingArea drawingArea=DEFAULT_DRAWING_AREA)
		{
			DrawingArea area=blitDrawingArea(image,destinationPosition,sourcePosition,blitSize,drawingArea);
			if(!area.isEmpty())
			{
				for(int y=area.min.y;y<=area.max.y;y++)
				{
					for(int x=area.min.x;x<=area.max.x;x++)
					{
						uint32_t color=image._getpixel(Pos(x,y)-destinationPosition+sourcePosition);
						if(color!=maskColor) internalPixels[y*internalSize.x+x]=color;
					}
				}
			}
		}
		void maskedBlitColor(const Image& image,uint32_t maskColor,uint32_t finalColor,const Pos& destinationPosition,const Pos& sourcePosition,const Pos& blitSize,
			DrawingArea drawingArea=DEFAULT_DRAWING_AREA)
		{
			DrawingArea area=blitDrawingArea(image,destinationPosition,sourcePosition,blitSize,drawingArea);
			if(!area.isEmpty())
			{
				for(int y=area.min.y;y<=area.max.y;y++)
				{
					for(int x=area.min.x;x<=area.max.x;x++)
					{
						uint32_t color=image._getpixel(Pos(x,y)-destinationPosition+sourcePosition);
						if(color!=maskColor) internalPixels[y*internalSize.x+x]=finalColor;
					}
				}
			}
		}
		void textprint(const Image& fontImage,const Pos& position,uint32_t color,const string& text,DrawingArea drawingArea=DEFAULT_DRAWING_AREA)
		{
			Pos characterSize=fontImage.size()/Pos(16,16);
			for(int c=0;c<text.size();c++)
			{
				uint32_t ch=uint32_t(uint8_t(text[c]));
				maskedBlitColor(fontImage,0x000000,color,position+Pos(characterSize.x*c,0),characterSize*Pos(ch%16,ch/16),characterSize,drawingArea);
			}
		}
	};
}
