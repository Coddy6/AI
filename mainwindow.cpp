#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QWheelEvent>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <random>
#include <QPointF>
#include <QMessageBox>

// Enumeración para representar las direcciones de zoom
enum ZoomDirection {
    ZoomIn,
    ZoomOut
};

int valorAnterior = 0;
// Atributo booleano para mantener el estado del botón
bool botonPresionado = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    reiniciarArchivo("Entradas.txt");
    CrearEscena();


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Convertir las coordenadas del evento de clic a las coordenadas de la escena
        QPointF scenePos = ui->graphicsView->mapToScene(event->pos());

        // Mostrar las coordenadas en la consola
        //qDebug() << "Coordenadas (x, y) del clic: " << scenePos.x() << ", " << scenePos.y();


        // Crear un punto gráfico en la posición del clic
        QGraphicsEllipseItem *point = new QGraphicsEllipseItem(scenePos.x() - 2, scenePos.y() -2, 4, 4); // Ajusta el tamaño del punto
        point->setBrush(Qt::yellow); // Color del punto
        ui->graphicsView->scene()->addItem(point);
        QList<QPointF> coordenadas;
        coordenadas << QPointF(scenePos.x(),scenePos.y());
        QString archivo = "Entradas.txt";
        guardarCoordenadasEnArchivo(archivo,coordenadas);
    }
}

//boton de mover entre la grafica
void MainWindow::on_pushButton_clicked(bool checked)
{
    if(checked == true)
    {
        ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        ui->pushButton->setStyleSheet("background-color: red;");
    }else{
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        ui->pushButton->setStyleSheet("background-color: blue;");
    }
}

//Slider para hacer zoom
void MainWindow::on_verticalSlider_valueChanged(int value)
{
    // Verificar si el valor del slider se incrementa o decrementa
    if (value > valorAnterior) {
        // Incremento: Zoom in
        qreal scaleFactor = 1.0 + (value - valorAnterior) / 100.0;
        ui->graphicsView->scale(scaleFactor,scaleFactor);
    } else if (value < valorAnterior) {
        // Decremento: Zoom out
        qreal scaleFactor = 1.0 / (1.0 + (valorAnterior - value) / 100.0);
        ui->graphicsView->scale(scaleFactor,scaleFactor);
    } else {
        // No hay cambio en el valor del slider
    }

    // Actualizar el valor anterior del slider
    valorAnterior = value;
}

void MainWindow::guardarCoordenadasEnArchivo(const QString &nombreArchivo, const QList<QPointF> &coordenadas) {
    // Crear un objeto QFile para el archivo
    QFile archivo(nombreArchivo);

    // Intentar abrir el archivo en modo de adjuntar el texto
    if (archivo.open(QIODevice::Append | QIODevice::Text)) {
        // Crear un objeto QTextStream para escribir en el archivo
        QTextStream out(&archivo);

        // Escribir cada par de coordenadas en el archivo, separadas por un delimitador
        for (const QPointF &punto : coordenadas) {
            out << punto.x() << "," << punto.y() << "\n";
        }

        // Cerrar el archivo después de escribir los datos
        archivo.close();
    } else {
        // Manejar el caso en que no se pueda abrir el archivo
        qDebug() << "No se pudo abrir el archivo para escribir";
    }
}


void MainWindow::reiniciarArchivo(const QString &nombreArchivo) {
    // Crear un objeto QFile para el archivo
    QFile archivo(nombreArchivo);

    // Intentar abrir el archivo en modo Truncate para borrar todo el contenido
    if (archivo.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        // Cerrar el archivo después de borrar todo el contenido
        archivo.close();
    } else {
        // Manejar el caso en que no se pueda abrir el archivo
        qDebug() << "No se pudo abrir el archivo para borrar";
    }
}

void MainWindow::on_Graficar_clicked()
{
    // Verificar el estado actual del botón
    if (botonPresionado == false) {
        botonPresionado = true;
        PlotNuevo();
    } else {
        bool confirmacion = mostrarConfirmacion("¿Quiere graficar de nuevo?");
        if (confirmacion) {
            eliminarLinea(ui->graphicsView,"linea1");
            PlotNuevo();
        } else {
            reiniciarArchivo("Entradas.txt");
            limpiarEscena(ui->graphicsView);
            reiniciarGrafica(ui->graphicsView);
            CrearEscena();
        }
    }
}

QVector<QPointF> MainWindow::leerCoordenadasDesdeArchivo(const QString &nombreArchivo) {
    QVector<QPointF> coordenadas;

    // Crear un objeto QFile para el archivo
    QFile archivo(nombreArchivo);

    // Intentar abrir el archivo en modo de lectura de texto
    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Crear un objeto QTextStream para leer desde el archivo
        QTextStream in(&archivo);

        // Leer el archivo línea por línea
        while (!in.atEnd()) {
            // Leer una línea del archivo
            QString linea = in.readLine();

            // Dividir la línea en coordenadas x e y utilizando el delimitador
            QStringList partes = linea.split(",");
            if (partes.size() == 2) {
                bool conversionOkX, conversionOkY;
                qreal x = partes[0].toDouble(&conversionOkX);
                qreal y = partes[1].toDouble(&conversionOkY);
                if (conversionOkX && conversionOkY) {
                    coordenadas.append(QPointF(x, y));
                } else {
                    qDebug() << "Error al convertir coordenadas:" << partes[0] << partes[1];
                }
            } else {
                qDebug() << "Error al leer línea:" << linea;
            }
        }

        // Cerrar el archivo después de leer los datos
        archivo.close();
    } else {
        // Manejar el caso en que no se pueda abrir el archivo
        qDebug() << "No se pudo abrir el archivo para leer";
    }

    return coordenadas;
}

QVector<double> MainWindow::convertirAVectorDouble(const QVector<QPointF> &vectorQPointF) {
    QVector<double> vectorDouble;
    for (const QPointF &punto : vectorQPointF) {
        vectorDouble.append(punto.x());
        vectorDouble.append(punto.y());
    }
    return vectorDouble;
}

QVector<double> MainWindow::multiplicarVectores(const QVector<double> &vector1, const QVector<double> &vector2) {
    QVector<double> resultado;

    // Verificar que el primer vector tenga solo dos columnas
    if (vector1.size() % 2 != 0) {
        qDebug() << "El primer vector debe tener un número par de elementos";
            return resultado;
    }

    // Multiplicar cada par de valores del primer vector por los valores del segundo vector
    for (int i = 0; i < vector1.size(); i += 2) {
        double producto1 = vector1[i] * vector2[0]; // Multiplicar por el primer valor del segundo vector
        double producto2 = vector1[i + 1] * vector2[1]; // Multiplicar por el segundo valor del segundo vector
        resultado.append(producto1+producto2+(1*ui->Peso0->value()));//suma de los productos y el bias
    }

    return resultado;
}

void MainWindow::cambiarColorPuntos(QGraphicsView *view, const QList<QPointF> &coordenadas, const QVector<int> &salida) {
    // Obtener la escena asociada al QGraphicsView
    QGraphicsScene *scene = view->scene();
    if (!scene) {
        qDebug() << "Error: No se pudo obtener la escena asociada al QGraphicsView.";
        return;
    }

    for (int i = 0; i < coordenadas.size(); ++i) {
        // Obtener las coordenadas y el valor de salida correspondiente
        QPointF coordenada = coordenadas.at(i);

        // Buscar todos los puntos en la escena que coincidan con las coordenadas dadas
        QList<QGraphicsItem*> items = scene->items(coordenada);
        for (QGraphicsItem *item : items) {
            // Verificar si el elemento es un punto
            if (item->type() == QGraphicsEllipseItem::Type) {
                QGraphicsEllipseItem *punto = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
                // Cambiar el color del punto según el valor de salida
                if (salida[i] == 1) {
                    punto->setBrush(Qt::blue); // Cambiar a azul si la salida es 1
                } else {
                    punto->setBrush(Qt::red); // Cambiar a rojo si la salida es 0
                }
            }
        }
    }
}

// Función para generar valores aleatorios para x en el rango [-maxX, maxX]
QVector<double> MainWindow::generarValoresX(int numPuntos, double maxX) {
    QVector<double> valoresX;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-maxX, maxX);
    for (int i = 0; i < numPuntos; ++i) {
        valoresX.append(dis(gen));
    }
    return valoresX;
}

// Función para calcular los valores de y correspondientes a los valores de x dados, usando la ecuación y = mx + c
QVector<double> MainWindow::calcularValoresY(const QVector<double> &valoresX, double pendiente, double ordenadaOrigen) {
    QVector<double> valoresY;
    for (double x : valoresX) {
        double y = pendiente * x + ordenadaOrigen;
        valoresY.append(y);
    }
    return valoresY;
}

// Función para graficar la línea que representa la pendiente en la escena dada
void MainWindow::graficarPendiente(QGraphicsView *view, double x1, double y1, double x2, double y2) {
    QGraphicsLineItem *linea = new QGraphicsLineItem(x1, y1, x2, y2);
    QGraphicsScene *scene = view->scene();
    linea->setData(Qt::UserRole, "linea1");
    scene->addItem(linea);
}

void MainWindow::on_Borrar_clicked()
{
    reiniciarArchivo("Entradas.txt");
    limpiarEscena(ui->graphicsView);
    reiniciarGrafica(ui->graphicsView);
    CrearEscena();
}


// Limpiar la escena del QGraphicsView
void MainWindow::limpiarEscena(QGraphicsView *graphicsView) {
    // Obtener la escena asociada al QGraphicsView
    QGraphicsScene *scene = graphicsView->scene();
    if (scene) {
        // Eliminar todos los elementos de la escena
        scene->clear();
    }
}

// Función para reiniciar la gráfica y sus valores
void MainWindow::reiniciarGrafica(QGraphicsView *graphicsView) {
    // Limpiar la escena
    limpiarEscena(graphicsView);
}

//Función para crear todo
void MainWindow::CrearEscena(){
    // Invertir la dirección del eje Y en la vista
    QTransform transform;
    transform.scale(1, -1);
    ui->graphicsView->setTransform(transform);
    // Centrar la vista en la escena
    ui->graphicsView->centerOn(0, 0);
    // Definir el tamaño de la ventana
    int windowSize = 500;

    // Ajustar el tamaño del Graphics View
    ui->graphicsView->setFixedSize(500, 500);

    // Crear una nueva escena y establecerla en el Graphics View
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    // Configurar la visualización en modo de desplazamiento infinito
    //ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    //ui->graphicsView->setDragMode(QGraphicsView::ScrollPerPixel);

    // Deshabilitar las barras de desplazamiento
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Habilitar la funcionalidad de zoom con la rueda del ratón
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setInteractive(true);
    ui->graphicsView->viewport()->installEventFilter(this);

    // Crear ejes X e Y
    QGraphicsLineItem *xAxis = new QGraphicsLineItem;
    xAxis->setLine(-windowSize, 0, windowSize, 0);
    //xAxis->setLine(-500, 0, 500, 0);
    scene->addItem(xAxis);

    QGraphicsLineItem *yAxis = new QGraphicsLineItem;
    yAxis->setLine(0, -windowSize, 0, windowSize);
    //yAxis->setLine(0, -500, 0, 500);
    scene->addItem(yAxis);



    // Ajustar la posición de las etiquetas en los ejes X e Y
    for (qreal i = -500; i <= 500; i += 50) {
        QGraphicsTextItem *labelX = new QGraphicsTextItem(QString::number(i));
        labelX->setPos(i, 20); // Ajustar la posición en el eje X
        labelX->setTransform(QTransform().scale(1, -1));
        labelX->setFont(QFont("Arial", 5)); // Establecer el tamaño de la fuente
        scene->addItem(labelX);

        QGraphicsTextItem *labelY = new QGraphicsTextItem(QString::number(i)); // Invertir la posición para el eje Y
        labelY->setPos(-20, i); // Ajustar la posición en el eje Y
        labelY->setTransform(QTransform().scale(1, -1));
        labelY->setFont(QFont("Arial", 5)); // Establecer el tamaño de la fuente
        scene->addItem(labelY);
    }
    ui->Graficar->setStyleSheet("background-color: green;");
    ui->Borrar->setStyleSheet("background-color: red;");
}

void MainWindow::PlotNuevo(){
    QVector<double> pesos = {ui->Peso1->value(),ui->Peso2->value()};
    QVector<double> Entradas = convertirAVectorDouble(leerCoordenadasDesdeArchivo("Entradas.txt"));
    QVector<QPointF> EntradasXY = leerCoordenadasDesdeArchivo("Entradas.txt");
    QVector<double> resultados = multiplicarVectores(Entradas,pesos);
    QVector<int> salida;
    for(int i = 0;i<resultados.size();i++)
    {
        if(resultados[i] >= 0)
        {
            salida.append(1);
        }else{
            salida.append(0);
        }
    }

    cambiarColorPuntos(ui->graphicsView,EntradasXY,salida);
    // Parámetros de la pendiente
    double m = (-(ui->Peso1->value()))/ui->Peso2->value(); // Pendiente
    double c = (-(ui->Peso0->value()))/ui->Peso2->value(); // Ordenada al origen

    // Generar valores aleatorios para x en el rango [0, 100]
    int numPuntos = 100; // Número de puntos a generar
    double maxX = 500.0;
    QVector<double> valoresX = generarValoresX(numPuntos, maxX);

    // Calcular los valores correspondientes de y usando la ecuación y = mx + c
    QVector<double> valoresY = calcularValoresY(valoresX, m, c);

    // Encontrar los puntos extremos para trazar la línea que representa la pendiente
    double minXValue = *std::min_element(valoresX.constBegin(), valoresX.constEnd());
    double maxXValue = *std::max_element(valoresX.constBegin(), valoresX.constEnd());
    double minYValue = *std::min_element(valoresY.constBegin(), valoresY.constEnd());
    double maxYValue = *std::max_element(valoresY.constBegin(), valoresY.constEnd());

    // Graficar la línea que representa la pendiente
    graficarPendiente(ui->graphicsView, minXValue, m * minXValue + c, maxXValue, m * maxXValue + c);
}

// Función para mostrar un cuadro de diálogo de confirmación
bool MainWindow::mostrarConfirmacion(const QString &mensaje) {
    QMessageBox::StandardButton respuesta;
    respuesta = QMessageBox::question(nullptr, "Confirmación", mensaje,QMessageBox::Yes|QMessageBox::No);
    return (respuesta == QMessageBox::Yes);
}


void MainWindow::eliminarLinea(QGraphicsView *view, const QString &identificador) {
    // Obtener todos los elementos en la escena
    QList<QGraphicsItem*> items = view->items();
    // Iterar sobre los elementos para encontrar la línea específica
    for (QGraphicsItem *item : items) {
        // Verificar si el elemento es una línea y si su identificador coincide
        if (item->type() == QGraphicsLineItem::Type && item->data(Qt::UserRole).toString() == identificador) {
            // Eliminar la línea de la escena
            view->scene()->removeItem(item);
            // Liberar la memoria asignada a la línea
            delete item;
            // Salir del bucle una vez que se haya eliminado la línea
            break;
        }
    }
}
