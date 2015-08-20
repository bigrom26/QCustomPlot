/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2015 Emanuel Eichhammer                            **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 25.04.15                                             **
**          Version: 1.3.1                                                **
****************************************************************************/
/*! \file */
#ifndef QCP_PLOTTABLE_GRAPH_H
#define QCP_PLOTTABLE_GRAPH_H

#include "../global.h"
#include "../axis/range.h"
#include "../plottable.h"
#include "../painter.h"

class QCPPainter;
class QCPAxis;

class QCP_LIB_DECL QCPGraphData
{
public:
  QCPGraphData();
  QCPGraphData(double key, double value);
  double key, value;
};
Q_DECLARE_TYPEINFO(QCPGraphData, Q_MOVABLE_TYPE);

template <class DataType>
inline bool qcpLessThanKey(const DataType &a, const DataType &b) { return a.key < b.key; }

template <class DataType>
class QCP_LIB_DECL QCPDataContainer
{
public:
  typedef typename QVector<DataType>::const_iterator const_iterator;
  typedef typename QVector<DataType>::iterator iterator;
  QCPDataContainer();
  
  void setAutoSqueeze(bool enabled);
  
  int size() const { return mData.size()-mPreallocSize; }
  bool isEmpty() const { return size() == 0; }
  bool autoSqueeze() const { return mAutoSqueeze; }
  
  void set(const QCPDataContainer<DataType> &data);
  void set(const QVector<DataType> &data, bool alreadySorted=false);
  void add(const QCPDataContainer<DataType> &data);
  void add(const QVector<DataType> &data, bool alreadySorted=false);
  void add(const DataType &data);
  void removeBefore(double key);
  void removeAfter(double key);
  void remove(double fromKey, double toKey);
  void remove(double key);
  void clear();
  void sort();
  void squeeze(bool preAllocation=true, bool postAllocation=true);
  
  QCPDataContainer::const_iterator constBegin() const { return mData.constBegin()+mPreallocSize; }
  QCPDataContainer::const_iterator constEnd() const { return mData.constEnd(); }
  QCPDataContainer::iterator begin() { return mData.begin()+mPreallocSize; }
  QCPDataContainer::iterator end() { return mData.end(); }
  QCPDataContainer::const_iterator findBeginBelowKey(double key) const;
  QCPDataContainer::const_iterator findEndAboveKey(double key) const;
  QCPRange keyRange(bool &foundRange, QCP::SignDomain signDomain=QCP::sdBoth);
  QCPRange valueRange(bool &foundRange, QCP::SignDomain signDomain=QCP::sdBoth);
  
protected:
  bool mAutoSqueeze;
  
  QVector<DataType> mData;
  int mPreallocSize;
  int mPreallocIteration;
  
  void preallocateGrow(int minimumPreallocSize);
  void performAutoSqueeze();
};


class QCP_LIB_DECL QCPGraphDataContainer : public QCPDataContainer<QCPGraphData>
{
public:
  QCPGraphDataContainer();
  // overrides which just call base class methods ("using" keyword would be better but breaks QtCreator autocomplete...hmm [QTCREATORBUG-14941]):
  inline void set(const QCPDataContainer<QCPGraphData> &data) { QCPDataContainer<QCPGraphData>::set(data); }
  inline void set(const QVector<QCPGraphData> &data, bool alreadySorted=false) { QCPDataContainer<QCPGraphData>::set(data, alreadySorted); }
  inline void add(const QCPDataContainer<QCPGraphData> &data) { QCPDataContainer<QCPGraphData>::set(data); }
  inline void add(const QVector<QCPGraphData> &data, bool alreadySorted=false) { QCPDataContainer<QCPGraphData>::add(data, alreadySorted); }
  inline void add(const QCPGraphData &data) { QCPDataContainer<QCPGraphData>::add(data); }
  // overloads which take explicit vectors:
  void set(const QVector<double> &keys, const QVector<double> &values, bool alreadySorted=false);
  void add(const QVector<double> &keys, const QVector<double> &values, bool alreadySorted=false);
};


class QCP_LIB_DECL QCPGraph : public QCPAbstractPlottable
{
  Q_OBJECT
  /// \cond INCLUDE_QPROPERTIES
  Q_PROPERTY(LineStyle lineStyle READ lineStyle WRITE setLineStyle)
  Q_PROPERTY(QCPScatterStyle scatterStyle READ scatterStyle WRITE setScatterStyle)
  Q_PROPERTY(QCPGraph* channelFillGraph READ channelFillGraph WRITE setChannelFillGraph)
  Q_PROPERTY(bool adaptiveSampling READ adaptiveSampling WRITE setAdaptiveSampling)
  /// \endcond
public:
  /*!
    Defines how the graph's line is represented visually in the plot. The line is drawn with the
    current pen of the graph (\ref setPen).
    \see setLineStyle
  */
  enum LineStyle { lsNone        ///< data points are not connected with any lines (e.g. data only represented
                                 ///< with symbols according to the scatter style, see \ref setScatterStyle)
                   ,lsLine       ///< data points are connected by a straight line
                   ,lsStepLeft   ///< line is drawn as steps where the step height is the value of the left data point
                   ,lsStepRight  ///< line is drawn as steps where the step height is the value of the right data point
                   ,lsStepCenter ///< line is drawn as steps where the step is in between two data points
                   ,lsImpulse    ///< each data point is represented by a line parallel to the value axis, which reaches from the data point to the zero-value-line
                 };
  Q_ENUMS(LineStyle)
  
  explicit QCPGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);
  virtual ~QCPGraph();
  
  // getters:
  QSharedPointer<QCPGraphDataContainer> data() const { return mDataContainer; }
  LineStyle lineStyle() const { return mLineStyle; }
  QCPScatterStyle scatterStyle() const { return mScatterStyle; }
  QCPGraph *channelFillGraph() const { return mChannelFillGraph.data(); }
  bool adaptiveSampling() const { return mAdaptiveSampling; }
  
  // setters:
  void setData(const QCPGraphDataContainer &data);
  void setData(QSharedPointer<QCPGraphDataContainer> data);
  void setData(const QVector<double> &keys, const QVector<double> &values, bool alreadySorted=false);
  void setLineStyle(LineStyle ls);
  void setScatterStyle(const QCPScatterStyle &style);
  void setChannelFillGraph(QCPGraph *targetGraph);
  void setAdaptiveSampling(bool enabled);
  
  // non-property methods:
  void addData(const QCPGraphDataContainer &data);
  void addData(const QCPGraphData &data);
  void addData(double key, double value);
  void addData(const QVector<double> &keys, const QVector<double> &values, bool alreadySorted=false);
  void removeDataBefore(double key);
  void removeDataAfter(double key);
  void removeData(double fromKey, double toKey);
  void removeData(double key);
  
  // reimplemented virtual methods:
  virtual void clearData();
  virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details=0) const;
  
protected:
  // property members:
  QSharedPointer<QCPGraphDataContainer> mDataContainer;
  LineStyle mLineStyle;
  QCPScatterStyle mScatterStyle;
  QPointer<QCPGraph> mChannelFillGraph;
  bool mAdaptiveSampling;
  
  // reimplemented virtual methods:
  virtual void draw(QCPPainter *painter);
  virtual void drawLegendIcon(QCPPainter *painter, const QRectF &rect) const;
  virtual QCPRange getKeyRange(bool &foundRange, QCP::SignDomain inSignDomain=QCP::sdBoth) const;
  virtual QCPRange getValueRange(bool &foundRange, QCP::SignDomain inSignDomain=QCP::sdBoth) const;
  
  // introduced virtual methods:
  virtual void drawFill(QCPPainter *painter, QVector<QPointF> *lineData) const;
  virtual void drawScatterPlot(QCPPainter *painter, QVector<QCPGraphData> *scatterData) const;
  virtual void drawLinePlot(QCPPainter *painter, QVector<QPointF> *lineData) const;
  virtual void drawImpulsePlot(QCPPainter *painter, QVector<QPointF> *lineData) const;
  
  // non-virtual methods:
  void getPreparedData(QVector<QCPGraphData> *lineData, QVector<QCPGraphData> *scatterData) const;
  void getPlotData(QVector<QPointF> *lineData, QVector<QCPGraphData> *scatterData) const;
  void getScatterPlotData(QVector<QCPGraphData> *scatterData) const;
  void getLinePlotData(QVector<QPointF> *linePixelData, QVector<QCPGraphData> *scatterData) const;
  void getStepLeftPlotData(QVector<QPointF> *linePixelData, QVector<QCPGraphData> *scatterData) const;
  void getStepRightPlotData(QVector<QPointF> *linePixelData, QVector<QCPGraphData> *scatterData) const;
  void getStepCenterPlotData(QVector<QPointF> *linePixelData, QVector<QCPGraphData> *scatterData) const;
  void getImpulsePlotData(QVector<QPointF> *linePixelData, QVector<QCPGraphData> *scatterData) const;
  void getVisibleDataBounds(QCPGraphDataContainer::const_iterator &lower, QCPGraphDataContainer::const_iterator &upper) const;
  void addFillBasePoints(QVector<QPointF> *lineData) const;
  void removeFillBasePoints(QVector<QPointF> *lineData) const;
  QPointF lowerFillBasePoint(double lowerKey) const;
  QPointF upperFillBasePoint(double upperKey) const;
  const QPolygonF getChannelFillPolygon(const QVector<QPointF> *lineData) const;
  int findIndexBelowX(const QVector<QPointF> *data, double x) const;
  int findIndexAboveX(const QVector<QPointF> *data, double x) const;
  int findIndexBelowY(const QVector<QPointF> *data, double y) const;
  int findIndexAboveY(const QVector<QPointF> *data, double y) const;
  double pointDistance(const QPointF &pixelPoint) const;
  
  friend class QCustomPlot;
  friend class QCPLegend;
};

#endif // QCP_PLOTTABLE_GRAPH_H
