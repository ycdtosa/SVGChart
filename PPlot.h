//  Modifications by David Kopec:
//  - Added SVGChart Namespace
//  - Fixed Modern g++ warnings
//  - Removed DummyPainter class
//
//  Copyright 2019 David Kopec
//
//  Permission is hereby granted, free of charge, to any person
//  obtaining a copy of this software and associated documentation files
//  (the "Software"), to deal in the Software without restriction,
//  including without limitation the rights to use, copy, modify, merge,
//  publish, distribute, sublicense, and/or sell copies of the Software,
//  and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice
//  shall be included in all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//  OTHER DEALINGS IN THE SOFTWARE.
//
//  Original Copyright Notice:

/***************************************************************************
 *                                                                         *
 *   Copyright notice:                                                     *
 *                                                                         *
 *   This is free Pier ware. You may do whatever you want with this code.  *
 *   You may cont(r)act me by email: pierphil@xs4all.nl                    *
 *                                                                         *
 ***************************************************************************/

#ifndef __PPLOT_H__
#define __PPLOT_H__

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <map>
using std::map;

namespace SVGChart {

    typedef vector<float> RealPlotData;

    struct PStyle {
      int mFontSize{10};
      string mFont;
      int mPenWidth{1};
      string mPenStyle;
      map<string,string> mVar;
    };

    class CalculatedDataBase {
    public:
        virtual float GetValue (long inIndex) const = 0;
        virtual long GetSize () const = 0;
    };

    // data
    class PlotDataBase {
     public:
      virtual ~PlotDataBase ();
      virtual const RealPlotData * GetRealPlotData () const = 0;
      virtual const CalculatedDataBase * GetCalculatedData () const {return nullptr;}
      long GetSize () const;
      float GetValue (long inIndex) const;

      virtual bool CalculateRange (float &outMin, float &outMax);
    };
    typedef vector<PlotDataBase *> PlotDataList;

    class PlotDataPointer: public PlotDataBase {
    public:
      PlotDataPointer (const PlotDataBase *inPlotData)
          : mPlotData (inPlotData)
        {}// does not own them

      virtual const RealPlotData * GetRealPlotData () const
        {return mPlotData->GetRealPlotData ();}

      virtual const CalculatedDataBase * GetCalculatedData () const 
        {return mPlotData->GetCalculatedData ();}

    private:
      const PlotDataBase *mPlotData;
    };

    // default data class
    class PlotData: public RealPlotData, public PlotDataBase {
     public:
      virtual const RealPlotData * GetRealPlotData () const
        {return this;}
    };

    class CalculatedData: public CalculatedDataBase {
    public:
        CalculatedData (float inMin, float inDelta, long inSize):
          mMin (inMin), mDelta (inDelta), mSize (inSize) {}
        virtual float GetValue (long inIndex) const { return mMin + inIndex * mDelta; }
        virtual long GetSize () const { return mSize; }

        float mMin;
        float mDelta;
        long mSize;
    };

    class CalculatedPlotData: public PlotDataBase {
    public:
        CalculatedPlotData (CalculatedDataBase* inCalculatedData):
          mCalculatedData (inCalculatedData) {}
        //~CalculatedPlotData () {delete mCalculatedData;}

        virtual const RealPlotData * GetRealPlotData () const {return nullptr;}
        virtual const CalculatedDataBase * GetCalculatedData () const {return mCalculatedData;}

        CalculatedDataBase* mCalculatedData;
    };

    class DummyData: public PlotDataBase {
    public:
      DummyData (long inSize=0);

      virtual const RealPlotData * GetRealPlotData () const
        {return &mRealPlotData;}

    private:
      RealPlotData mRealPlotData;
    };

    class StringData: public PlotDataBase {
    public:
      void AddItem (const char *inString);

      const vector<string> * GetStringData () const
        {return &mStringData;}

      virtual const RealPlotData * GetRealPlotData () const
        {return &mRealPlotData;}

    private:
      RealPlotData mRealPlotData;
      vector<string> mStringData;
    };


    float SafeLog (float inFloat, float inBase, float inFac=1);
    float SafeExp (float inFloat, float inBase, float inFac=1);

    class PRect {
     public:
      PRect () = default;
      long mX{0};
      long mY{0};
      long mW{0};
      long mH{0};
    };

    class PMargins {
     public:
      PMargins () = default;
      PMargins (long inLeft, long inRight, long inTop, long inBottom)
          : mLeft (inLeft), mRight (inRight), mTop(inTop), mBottom (inBottom) 
        {}

      long mLeft{0};
      long mRight{0};
      long mTop{0};
      long mBottom{0};
    };

    class PColor {
     public:
      PColor () = default;
      PColor (int inR, int inG, int inB)
          : mR(inR), mG(inG), mB(inB)
        {}
  
      unsigned char mR{0};
      unsigned char mG{0};
      unsigned char mB{0};
    };

    class LegendData {
    public:
      LegendData () = default;
      string mName;
      PColor mColor;
      bool mShow{ true };

      void SetDefaultColor (int inPlotIndex);
      void SetDefaultValues (int inPlotIndex);
      static PColor GetDefaultColor (int inPlotIndex);
      PStyle mStyle;
    };

    typedef vector<LegendData *> LegendDataList;

    class PlotDataSelection: public vector<int> {
    public:
      PlotDataSelection (long inSize=0)
          : vector<int>(inSize)
        {}

      bool IsSelected (long inIndex) const;
      long GetSelectedCount () const;
    };

    typedef vector<PlotDataSelection *> PlotDataSelectionList;

    class Painter {
     public:

      virtual void DrawLine (float inX1, float inY1, float inX2, float inY2)=0;
      virtual void FillRect (int inX, int inY, int inW, int inH)=0;
      virtual void InvertRect (int inX, int inY, int inW, int inH)=0;
      virtual void SetClipRect (int inX, int inY, int inW, int inH)=0;
      virtual long GetWidth () const=0;
      virtual long GetHeight () const=0;
      virtual void SetLineColor (int inR, int inG, int inB)=0;
      virtual void SetFillColor (int inR, int inG, int inB)=0;
      virtual long CalculateTextDrawSize (const char *inString)=0;
      virtual long GetFontHeight () const =0;
      virtual void DrawText (int inX, int inY, const char *inString)=0;
      virtual void DrawRotatedText (int inX, int inY, float inDegrees, const char *inString)=0;
      virtual void SetStyle ([[maybe_unused]] const PStyle &inStyle)
        {}
    };

    class Trafo;
    class AxisSetup;

    class DataDrawerBase {
     public:
      DataDrawerBase () = default;
      virtual ~DataDrawerBase () = default;

      void SetXTrafo (Trafo *inTrafo) {mXTrafo = inTrafo;}
      void SetYTrafo (Trafo *inTrafo) {mYTrafo = inTrafo;}
      void SetDrawFast (bool inDrawFast) {mDrawFast = inDrawFast;}
      void SetPlotCount (int inPlotCount) {mPlotCount = inPlotCount;}
      void SetPlotIndex (int inPlotIndex) {mPlotIndex = inPlotIndex;}

      virtual bool DrawData (const PlotDataBase &inXData, const PlotDataBase &inYData, const PlotDataSelection &inPlotDataSelection, const AxisSetup &inXAxisSetup, const PRect &inRect, Painter &inPainter) const =0;

      virtual DataDrawerBase* Clone () const = 0;
     protected:
      Trafo *mXTrafo{nullptr};
      Trafo *mYTrafo{nullptr};
      bool  mDrawFast{false};
      int   mPlotCount{1};
      int   mPlotIndex{0};
    };

    typedef vector<DataDrawerBase *> DataDrawerList;

    class LineDataDrawer: public DataDrawerBase {
     public:
      LineDataDrawer () = default;
      LineDataDrawer ( bool DrawLine, bool DrawPoint ):
        mDrawLine( DrawLine )
        ,mDrawPoint( DrawPoint )
      {}

      virtual bool DrawData (const PlotDataBase &inXData, const PlotDataBase &inYData, const PlotDataSelection &inPlotDataSelection, const AxisSetup &inXAxisSetup, const PRect &inRect, Painter &inPainter) const;

      virtual DataDrawerBase* Clone () const;
      virtual bool DrawPoint (int inScreenX, int inScreenY, const PRect &inRect, Painter &inPainter) const;
      virtual bool DrawSelection (int inScreenX, int inScreenY, const PRect &inRect, Painter &inPainter) const;

      bool mDrawLine{ true };
      bool mDrawPoint{ false };
      PStyle mStyle;
    };

    class DotDataDrawer: public LineDataDrawer {
     public:
       DotDataDrawer (): LineDataDrawer( false, true ) {}

      virtual bool DrawPoint (int inScreenX, int inScreenY, const PRect &inRect, Painter &inPainter) const;
    };

    class BarDataDrawer: public DataDrawerBase {
     public:
      BarDataDrawer() = default;
      BarDataDrawer( bool inDrawOnlyLastPoint )
          :mDrawOnlyLastPoint (inDrawOnlyLastPoint)
        {}

      virtual bool DrawData (const PlotDataBase &inXData, const PlotDataBase &inYData, const PlotDataSelection &inPlotDataSelection, const AxisSetup &inXAxisSetup, const PRect &inRect, Painter &inPainter) const;
      virtual DataDrawerBase* Clone () const;

     protected:
      bool mDrawOnlyLastPoint{ false }; // special mode
      virtual bool DrawOnlyLastPoint (const PlotDataBase &inXData, const PlotDataBase &inYData, const PlotDataSelection &inPlotDataSelection, const AxisSetup &inXAxisSetup, const PRect &inRect, Painter &inPainter) const;
    };


    class PlotDataContainer {
     public:
      PlotDataContainer ();
      ~PlotDataContainer ();

      void RemoveElement (int inIndex);
      void ClearData ();

      void AddXYPlot (PlotDataBase *inXData, PlotDataBase *inYData, LegendData *inLegendData=nullptr, DataDrawerBase *inDataDrawer=nullptr, PlotDataSelection *inPlotDataSelection=nullptr);//takes ownership
      void SetXYPlot (int inIndex, PlotDataBase *inXData, PlotDataBase *inYData, LegendData *inLegendData=nullptr, DataDrawerBase *inDataDrawer=nullptr, PlotDataSelection *inPlotDataSelection=nullptr);//takes ownership

      int GetPlotCount () const
        {return mYDataList.size ();}

      PlotDataBase * GetXData (int inIndex);
      PlotDataBase * GetYData (int inIndex);
      LegendData * GetLegendData (int inIndex);
      DataDrawerBase * GetDataDrawer (int inIndex);
      PlotDataSelection * GetPlotDataSelection (int inIndex);
      bool  SetDataDrawer (int inIndex, DataDrawerBase* inDataDrawer);  // takes ownership

      int GetPlotIndexByName (const string &inName) const;// negative value: not found

      const PlotDataBase * GetConstXData (int inIndex) const;
      const PlotDataBase * GetConstYData (int inIndex) const;
      const LegendData * GetConstLegendData (int inIndex) const;
      const DataDrawerBase * GetConstDataDrawer (int inIndex) const;
      const PlotDataSelection * GetConstPlotDataSelection (int inIndex) const;

      bool CalculateXRange (float &outXMin, float &outXMax) const;
      bool CalculateYRange (float inXMin, float inXMax, float &outYMin, float &outYMax) const;
      bool CalculateYRangePlot (float inXMin, float inXMax, const PlotDataBase &inXData, const PlotDataBase &inYData, float &outYMin, float &outYMax) const;

     protected:
      bool CheckState () const;
      PlotDataList mXDataList;
      PlotDataList mYDataList;
      LegendDataList mLegendDataList;
      DataDrawerList mDataDrawerList;
      PlotDataSelectionList mPlotDataSelectionList;
    };

    class GridInfo {
    public:
        GridInfo () = default;
        GridInfo (const bool inXGridOn, const bool inYGridOn )
            : mXGridOn (inXGridOn), mYGridOn (inYGridOn)
          {}
        
        bool mXGridOn{false};
        bool mYGridOn{false};
    };

    class TickInfo {
     public:
      TickInfo () = default;
      static float RoundSpan (float inSpan);

      static void MakeFormatString (float inValue, string &outFormatString);

      bool mAutoTick{ true };
      bool mAutoTickSize{ true };
      bool mTicksOn{ true };
      
      int mTickDivision{ 1 };
      float mMajorTickSpan{ 1 }; // in plot units
      int mMajorTickScreenSize{ 1 };
      int mMinorTickScreenSize{ 1 };
      string mFormatString{ "%.0f" };
      PStyle mStyle;
    };

    class AxisSetup {

     public:
      AxisSetup () = default;

      void SetMin (float inMin) {mMin = inMin;}
      void SetMax (float inMax) {mMax = inMax;}
      void SetAutoScale (bool inBool) {mAutoScaleMin = mAutoScaleMax = inBool;}
      bool IsAutoScale () const {return mAutoScaleMin && mAutoScaleMax;}

      float mMin{ 0 };
      float mMax{ 0 };
      bool mAutoScaleMin{ true };
      bool mAutoScaleMax{ true };
      bool mAscending{ true };  // not Ascending: Descending
      bool mLogScale{ false };
      bool mCrossOrigin{ true };
      long mMaxDecades{ -1 };// property for auto logscale
      long mLogFactor{ 1 };// to make db possible with logscale
      float mLogBase{ 10 };

      string mLabel;
      PStyle mStyle;

      TickInfo mTickInfo;
    };

    class Trafo {
     public:
      virtual ~Trafo () = default;
      virtual float Transform (float inValue) const=0;
      virtual float TransformBack (float inValue) const = 0;
    };
    class LinTrafo: public Trafo {
     public:
      LinTrafo () = default;

      virtual float Transform (float inValue) const;
      virtual float TransformBack (float inValue) const;

      float mOffset{ 0 };
      float mSlope{ 0 };
    };

    class LogTrafo: public Trafo {
     public:
      LogTrafo () = default;
      virtual float Transform (float inValue) const;
      virtual float TransformBack (float inValue) const;

      float mOffset{ 0 };
      float mSlope{ 0 };
      float mBase{ 10 };
      float mFactor{ 1 };
    };

    class TickIterator {
    public:
      TickIterator () = default;
      virtual ~TickIterator () = default;

      virtual bool Init ()=0;
      virtual bool GetNextTick (float &outTick, bool &outIsMajorTick, string &outFormatString)=0;

      virtual bool InitFromRanges (float inParRange, float inOrthoScreenRange, float inDivGuess, TickInfo &outTickInfo) const=0;
      virtual bool AdjustRange ([[maybe_unused]] float &ioMin, [[maybe_unused]] float &ioMax) const
        {return true;}

      void SetAxisSetup (const AxisSetup *inAxisSetup) {mAxisSetup = inAxisSetup;}

    protected:
      const AxisSetup *mAxisSetup{ nullptr };
    };

    class LinTickIterator: public TickIterator {
    public:
      LinTickIterator () = default;
      virtual bool Init ();
      virtual bool GetNextTick (float &outTick, bool &outIsMajorTick, string &outFormatString);

      bool InitFromRanges (float inParRange, float inOrthoScreenRange, float inDivGuess, TickInfo &outTickInfo) const;
    protected:
      float mCurrentTick{ 0 };
      long mCount{ 0 }; //was uninitialized
      float mDelta{ 0 };
      string mFormatString;
    };

    class LogTickIterator: public TickIterator {
    public:
      LogTickIterator () = default;
      virtual bool Init ();
      virtual bool GetNextTick (float &outTick, bool &outIsMajorTick, string &outFormatString);

      bool InitFromRanges (float inParRange, float inOrthoScreenRange, float inDivGuess, TickInfo &outTickInfo) const;
      virtual bool AdjustRange (float &ioMin, float &ioMax) const;
      float RoundUp (float inFloat) const;
      float RoundDown (float inFloat) const;
    protected:
      float mCurrentTick{ 0 };
      long mCount{ 0 }; //was uninitialized
      float mDelta{ 0 };
    };

    class NamedTickIterator: public LinTickIterator {
    public:
      NamedTickIterator () = default;

      void SetStringList (const vector<string> &inStringList)
        {mStringList = inStringList;}

      //  virtual bool Init ();
      virtual bool GetNextTick (float &outTick, bool &outIsMajorTick, string &outFormatString);

      bool InitFromRanges (float inParRange, float inOrthoScreenRange, float inDivGuess, TickInfo &outTickInfo) const;
    protected:
      vector<string> mStringList;
    };

    class PlotBackground {
     public:
      PlotBackground () = default;
      bool mTransparent{ true };
      PColor mPlotRegionBackColor{ 255, 255, 255 };
      string mTitle;
      PStyle mStyle;
    };

    class PPlot;

    class PDrawer {
     public:
      typedef vector<PDrawer *> tList;

      virtual ~PDrawer () = default;
      virtual bool Prepare ([[maybe_unused]] Painter &inPainter, [[maybe_unused]] PPlot& inPPlot) {return true;}
      virtual bool Draw (Painter &inPainter)=0;
    };

    class PCalculator {// base class to do additional calculations on a PPlot
     public:
      typedef vector<PCalculator *> tList;

      virtual ~PCalculator () = default;

      virtual bool ShouldCalculate () const 
        {return true;}
      
      virtual bool Calculate ([[maybe_unused]] Painter &inPainter, [[maybe_unused]] PPlot& inPPlot)
        {return true;}
    };


    class PainterTester: public PDrawer {
     public:
        virtual bool Draw (Painter &inPainter);
    };

    class PPlot: public PDrawer {
     public:
      PPlot ();
      virtual ~PPlot ();

      virtual bool Draw (Painter &inPainter);

      PlotDataContainer mPlotDataContainer;
      AxisSetup mXAxisSetup;
      AxisSetup mYAxisSetup;
      GridInfo  mGridInfo;
      PMargins mMargins;// [pixels]
      PlotBackground mPlotBackground;

      void SetPPlotDrawer (PDrawer *inPDrawer);// taker ownership. Used to bypass normal Draw function, i.e., set Draw function by composition.
      void SetPPlotDrawer (PDrawer &inPDrawer);// same as above: does not take ownership

      bool mHasAnyModifyingCalculatorBeenActive{ false };
      PCalculator::tList mModifyingCalculatorList;
      PCalculator::tList mPostCalculatorList;
      PDrawer::tList mPreDrawerList;
      PDrawer::tList mPostDrawerList;

      TickIterator *mXTickIterator;
      TickIterator *mYTickIterator;

      virtual bool CalculateXTransformation (const PRect &inRect);
      virtual bool CalculateYTransformation (const PRect &inRect);
      virtual bool DrawGridXAxis (const PRect &inRect, Painter &inPainter) const;
      virtual bool DrawGridYAxis (const PRect &inRect, Painter &inPainter) const;
      virtual bool DrawXAxis (const PRect &inRect, Painter &inPainter) const;
      virtual bool DrawYAxis (const PRect &inRect, Painter &inPainter) const;
      virtual bool CalculateTickInfo (const PRect &inRect, Painter &inPainter);

      Trafo *mXTrafo;
      Trafo *mYTrafo;

      static int Round (float inFloat);
      static const float kRangeVerySmall;
     protected:
      PPlot (const PPlot&);
      PPlot& operator=(const PPlot&);

      static bool CalculateLogTransformation (int inBegin, int inEnd, const AxisSetup& inAxisSetup, LogTrafo& outTrafo);
      static bool CalculateLinTransformation (int inBegin, int inEnd, const AxisSetup& inAxisSetup, LinTrafo& outTrafo);

      virtual bool DrawPlotBackground (const PRect &inRect, Painter &inPainter) const;
      virtual bool DrawXTick (float inX, int inScreenY, bool inMajor, const string &inFormatString, Painter &inPainter, PRect &outRect) const;
      virtual bool DrawYTick (float inY, int inScreenX, bool inMajor, const string &inFormatString, Painter &inPainter, PRect &outRect) const;
      virtual bool DrawLegend (const PRect &inRect, Painter &inPainter) const;
      virtual bool DrawPlot (int inIndex, const PRect &inRect, Painter &inPainter) const;
      virtual bool ConfigureSelf ();// change here implementations of interfaces
      virtual bool ValidateData ();// check preconditions here things like x is ascending
      virtual bool CalculateAxisRanges ();
      virtual bool CheckRange (const AxisSetup &inAxisSetup) const;

      void SetTickSizes (int inFontHeight, TickInfo &ioTickInfo);

      // trafo's between plot coordinates and screen coordinates.
      LinTrafo mXLinTrafo;
      LinTrafo mYLinTrafo;
      LogTrafo mXLogTrafo;
      LogTrafo mYLogTrafo;

      LinTickIterator mXLinTickIterator;
      LinTickIterator mYLinTickIterator;
      LogTickIterator mXLogTickIterator;
      LogTickIterator mYLogTickIterator;
      NamedTickIterator mXNamedTickIterator;

      PDrawer * mPPlotDrawer{ nullptr };
      bool mOwnsPPlotDrawer{ true };
    };

    bool MakeExamplePlot (int inExample, PPlot &ioPPlot);
    void MakeExamplePlot1 (PPlot &ioPPlot);
    void MakeExamplePlot2 (PPlot &ioPPlot);
    void MakeExamplePlot3 (PPlot &ioPPlot);
    void MakeExamplePlot4 (PPlot &ioPPlot);
    void MakeExamplePlot5 (PPlot &ioPPlot);
    void MakeExamplePlot6 (PPlot &ioPPlot);
    void MakeExamplePlot7 (PPlot &ioPPlot);
    void MakeExamplePlot8 (PPlot &ioPPlot);
    void MakePainterTester (PPlot &ioPPlot);

    void MakeCopy (const PPlot &inPPlot, PPlot &outPPlot);

    // following functions can be used to interface with scripts
    void SetCurrentPPlot (PPlot *inPPlot);
    PPlot & GetCurrentPPlot ();
}

#endif
