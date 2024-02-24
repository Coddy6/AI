#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qgraphicsscene.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void mousePressEvent(QMouseEvent *event);
    void scaleView(qreal scaleFactor);
    void guardarCoordenadasEnArchivo(const QString &nombreArchivo, const QList<QPointF> &coordenadas);
    void reiniciarArchivo(const QString &nombreArchivo);
    QVector<QPointF> leerCoordenadasDesdeArchivo(const QString &nombreArchivo);
    QVector<double> convertirAVectorDouble(const QVector<QPointF> &vectorQPointF);
    QVector<double> multiplicarVectores(const QVector<double> &vector1, const QVector<double> &vector2);
    void cambiarColorPuntos(QGraphicsView *view, const QList<QPointF> &coordenadas, const QVector<int> &salida);
    QVector<double> generarValoresX(int numPuntos, double maxX);
    QVector<double> calcularValoresY(const QVector<double> &valoresX, double pendiente, double ordenadaOrigen);
    void graficarPendiente(QGraphicsView *view, double x1, double y1, double x2, double y2);
    void limpiarEscena(QGraphicsView *graphicsView);
    void reiniciarGrafica(QGraphicsView *graphicsView);
    void CrearEscena();
    void PlotNuevo();
    bool mostrarConfirmacion(const QString &mensaje);
    void eliminarLinea(QGraphicsView *view, const QString &identificador);

private slots:

    void on_pushButton_clicked(bool checked);

    void on_verticalSlider_valueChanged(int value);

    void on_Graficar_clicked();

    void on_Borrar_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
};
#endif // MAINWINDOW_H
