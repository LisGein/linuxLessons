#pragma once

#include <QMainWindow>
#include "datamodel.h"

typedef std::pair<int, int> coordinates;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QImage *image_;
    QImage texture_;
    const size_t width_;
    const size_t height_;
    const size_t depth_;
    QVector<vector_3i> faces_;
    QVector<point_3f> verts_;
    QVector<point_2f> vt_;

    void line(int x0, int y0, int x1, int y1, int r, int g, int b);
    void draw_face();
    void triagle(point_3i screen_coords[], float intensity, int *zbuffer, point_2i tex_coords[]);
};

