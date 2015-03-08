#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <QObject>
#include <QVector2D>
#include <vector>

class Simulator : public QObject
{
    Q_OBJECT
public:
    Simulator(QObject* parent = 0);
    ~Simulator();

public slots:
    void step();

signals:
    void stepCompleted();
};

#endif // SIMULATOR_H
