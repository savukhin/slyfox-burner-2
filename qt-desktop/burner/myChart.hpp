#ifndef MYCHART_HPP
#define MYCHART_HPP

#include <QtCharts/QChart>
#include <QChart>
#include <QtCharts>

using namespace QtCharts;

class MyChart {
private:
    QChart *chart = new QChart;
    QLineSeries* series = new QLineSeries();

    QValueAxis *axisX = new QValueAxis;
    QValueAxis *axisY = new QValueAxis;

    void checkExpandAxis(qreal value, QValueAxis *axis) {
        if (value < axis->min()) {
            axis->setRange(value - 10, axis->max());
        } else if (value > axis->max()) {
            axis->setRange(axis->min(), value + 10);
        }
    }

    QTime startTime;

public:
    MyChart(QChartView *chartsView, QString label, QString legend) {
        chartsView->setChart(chart);
        this->chart->setTitle(label);

        axisX->setRange(0, 10);
//        axisX->setLabelFormat("%.2f");
        axisX->setTitleText("Time since start, msec");

        axisY->setRange(0, 10);
    //    axisY->setRange(-2.15, 2.15);
    //    axisY->setTickCount(3);
        axisY->setLabelFormat("%g");

        this->series->setName(legend);

        this->chart->addSeries(this->series);

        this->chart->addAxis(axisX, Qt::AlignBottom);
        this->chart->addAxis(axisY, Qt::AlignLeft);

        this->series->attachAxis(axisX);
        this->series->attachAxis(axisY);

        this->series->append(0, 10);
        this->series->append(1, 20);
        this->series->append(2, 10);

        startTime = QTime::currentTime();
    }

    void append(qreal value) {
        long time = this->startTime.msecsTo(QTime::currentTime());
        this->series->append(time, value);
        checkExpandAxis(value, axisY);
        checkExpandAxis(time, axisX);
    }
};

#endif // MYCHART_HPP
