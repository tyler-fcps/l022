#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <random>
#include <string>

using namespace std;

namespace ppm
{
    class Image
    {
    public:
        Image() {}
        Image(string name) : width(800), height(800), name(name)
        {
            for (int i = 0; i < width; i++)
            {
                for (int j = 0; j < height * 3; j++)
                {
                    imageData[i][j] = 0;
                }
            }
        }
        void output()
        {
            // Create and open a text file
            ofstream out(this->name);

            // Write header to file
            out << "P3 " << this->width << " " << this->height << " 255\r\n";

            // Write data
            for (int i = 0; i < this->width; i++)
            {
                for (int j = 0; j < this->height * 3; j++)
                {
                    out << this->imageData[i][j] << " ";
                }
                out << "\n";
            }

            // Close the file
            out.close();
        }
        void write(int x, int y, int r, int g, int b)
        {
            if (x < 0 || y < 0 || x >= this->width || y >= this->height)
            {
                return;
            }

            this->imageData[y][x * 3 + 0] = r;
            this->imageData[y][x * 3 + 1] = g;
            this->imageData[y][x * 3 + 2] = b;
        }
        int get_width()
        {
            return this->width;
        }
        int get_height()
        {
            return this->height;
        }

    private:
        int width;
        int height;
        std::string name;
        unsigned int imageData[800][800 * 3];
    };

    class Point
    {
    public:
        Point() {}

        Point(double x, double y) : x(x), y(y) {}

        double xpos()
        {
            return this->x;
        }

        double ypos()
        {
            return this->y;
        }

        void set_xpos(double x)
        {
            this->x = x;
        }

        void set_ypos(double y)
        {
            this->y = y;
        }
        double calc_dist(Point &other)
        {
            double dx, dy, dist;
            dx = this->x - other.xpos();
            dy = this->y - other.ypos();
            dist = sqrt(dx * dx + dy * dy);
            return dist;
        }

    private:
        double x, y;
    };

    class Line
    {
    public:
        Line(double x1, double y1, double x2, double y2) : x1(x1), x2(x2), y1(y1), y2(y2)
        {
        }
        void draw(Image *image, int r, int g, int b)
        {
            double delta_x = this->x2 - this->x1;
            double delta_y = this->y2 - this->y1;

            if (delta_x * delta_x > delta_y * delta_y)
            {
                if (delta_x < 0)
                {
                    this->major_x(image, r, g, b, this->x2, this->y2, this->x1, this->y1);
                }
                else
                {
                    this->major_x(image, r, g, b, this->x1, this->y1, this->x2, this->y2);
                }
            }
            else
            {
                if (delta_y < 0)
                {
                    this->major_y(image, r, g, b, this->x2, this->y2, this->x1, this->y1);
                }
                else
                {
                    this->major_y(image, r, g, b, this->x1, this->y1, this->x2, this->y2);
                }
            }
        }
        Point calc_midpoint()
        {
            auto x = (this->x1 + this->x2) / 2;
            auto y = (this->y1 + this->y2) / 2;
            return Point(x, y);
        }
        double calc_length()
        {
            return sqrt((this->x2 - this->x1) * (this->x2 - this->x1) + (this->y2 - this->y1) * (this->y2 - this->y1));
        }
        double calc_slope()
        {
            return (this->y2 - this->y1) / (this->x2 - this->x1);
        }
        double x1_pos()
        {
            return this->x1;
        }
        double y1_pos()
        {
            return this->y1;
        }
        double x2_pos()
        {
            return this->x2;
        }
        double y2_pos()
        {
            return this->y2;
        }
        void extend(double min_x, double max_x)
        {
            double slope = (this->y2 - this->y1) / (this->x2 - this->x1);
            double min_y = slope * (min_x - this->x1) + this->y1;
            double max_y = slope * (max_x - this->x1) + this->y1;
            this->x1 = min_x;
            this->y1 = min_y;
            this->x2 = max_x;
            this->y2 = max_y;
        }
        Point calc_intersect(Line &other)
        {
            double x, y, s1, s2;
            s1 = this->calc_slope();
            s2 = other.calc_slope();
            x = (s1 * this->x1 - s2 * other.x1_pos() + other.y1_pos() - this->y1) / (s1 - s2);
            y = s1 * (x - this->x1) + this->y1;
            return Point(x, y);
        }

    private:
        double x1, x2, y1, y2;

        void major_x(Image *image, int r, int g, int b, double x1d, double y1d, double x2d, double y2d)
        {
            double width, height;
            width = (double)image->get_width();
            height = (double)image->get_height();
            int x1, y1, x2, y2;
            x1 = (int)(width * x1d);
            y1 = (int)(height * y1d);
            x2 = (int)(width * x2d);
            y2 = (int)(height * y2d);

            int delta_x = x2 - x1;
            int delta_y = y2 - y1;
            int dir = 1;
            if (delta_y < 0)
            {
                dir = -1;
                delta_y = -delta_y;
            }
            int error = delta_y - delta_x;
            int y = y1;

            for (int x = x1; x < x2; x++)
            {
                image->write(x, y, r, g, b);

                if (error >= 0)
                {
                    y += dir;
                    error -= delta_x;
                }

                error += delta_y;
            }
        }
        void major_y(Image *image, int r, int g, int b, double x1d, double y1d, double x2d, double y2d)
        {
            double width, height;
            width = (double)image->get_width();
            height = (double)image->get_height();
            int x1, y1, x2, y2;
            x1 = (int)(width * x1d);
            y1 = (int)(height * y1d);
            x2 = (int)(width * x2d);
            y2 = (int)(height * y2d);

            int delta_x = x2 - x1;
            int delta_y = y2 - y1;
            int dir = 1;
            if (delta_x < 0)
            {
                dir = -1;
                delta_x = -delta_x;
            }

            int error = delta_x - delta_y;
            int x = x1;

            for (int y = y1; y < y2; y++)
            {
                image->write(x, y, r, g, b);

                if (error >= 0)
                {
                    x += dir;
                    error -= delta_y;
                }

                error += delta_x;
            }
        }
    };

    class Circle
    {
    public:
        Circle(double x, double y, double r) : x(x), y(y), r(r)
        {
        }
        void draw(Image *image, int r, int g, int b)
        {
            int x, y, y2, y2_new, ty, xoff, yoff;
            x = 0;
            y = (int)(this->r * (double)image->get_width());
            y2 = y * y;
            y2_new = y2;
            ty = (2 * y) - 1;
            xoff = (this->x * (double)image->get_width());
            yoff = (this->y * (double)image->get_height());

            cout << y << " " << xoff << " " << yoff << " " << this->r << endl;

            while (x <= y)
            {
                if ((y2 - y2_new) >= ty)
                {
                    y2 -= ty;
                    y -= 1;
                    ty -= 2;
                }
                image->write(xoff + x, yoff + y, r, g, b);
                image->write(xoff + x, yoff - y, r, g, b);
                image->write(xoff - x, yoff + y, r, g, b);
                image->write(xoff - x, yoff - y, r, g, b);
                image->write(xoff + y, yoff + x, r, g, b);
                image->write(xoff + y, yoff - x, r, g, b);
                image->write(xoff - y, yoff + x, r, g, b);
                image->write(xoff - y, yoff - x, r, g, b);
                y2_new -= (2 * x) - 3;
                x += 1;
            }
        }
        double radius()
        {
            return this->r;
        }
        double x_pos()
        {
            return this->x;
        }
        double y_pos()
        {
            return this->y;
        }

    private:
        double x, y, r;
    };
}

namespace gen_quad
{
    using ppm::Point;

    bool in_triangle(Point &p0, Point &p1, Point &p2, Point &p3)
    {
        auto sign = [](Point &p1, Point &p2, Point &p3)
        {
            return (p1.xpos() - p3.xpos()) * (p2.ypos() - p3.ypos()) - (p2.xpos() - p3.xpos()) * (p1.ypos() - p3.ypos());
        };

        double l1, l2, l3;
        l1 = sign(p0, p1, p2);
        l2 = sign(p0, p2, p3);
        l3 = sign(p0, p3, p1);

        bool neg, pos;
        neg = (l1 < 0) || (l2 < 0) || (l3 < 0);
        pos = (l1 > 0) || (l2 > 0) || (l3 > 0);

        return !neg || !pos;
    }

    void gen_quad()
    {
        // Open log
        ofstream log("log.txt");
        // Make vecs
        auto p = new Point[4];
        // Generate random numbers
        random_device os_seed;
        mt19937 gen(os_seed());
        uniform_real_distribution<> xy(0, 1);
        // Printing lambda
        auto print_vec = [&](Point &point)
        { log << "(" << point.xpos() << ", " << point.ypos() << ")"; };

        log << "Inital Points: ";
        for (int i = 0; i < 4; i++)
        {
            p[i] = Point(xy(gen), xy(gen));
            log << "p" << i << " = ";
            print_vec(p[i]);
            log << ", ";
        }
        log << endl;

        int count = 0;
        for (int i = 0; i < 4; i++)
        {
            while (in_triangle(p[(i + 0) % 4], p[(i + 1) % 4], p[(i + 2) % 4], p[(i + 3) % 4]))
            {
                log << "Failed (" << count << "), point p" << i << " is inside the rest" << endl;
                p[3] = Point(xy(gen), xy(gen));
                log << "New p3 is ";
                print_vec(p[3]);
                log << endl;
                count++;
                i = 0;
            }
        }

        // Create and open a text file
        ofstream out("points.txt");

        // Write data
        out << fixed << setprecision(17) << "("
            << p[0].xpos() << "," << p[0].ypos() << ") , ("
            << p[1].xpos() << "," << p[1].ypos() << ") , ("
            << p[2].xpos() << "," << p[2].ypos() << ") , ("
            << p[3].xpos() << "," << p[3].ypos() << ")" << endl;

        // Write for desmos
        log << "Final Points:" << endl;
        for (int i = 0; i < 4; i++)
        {
            print_vec(p[i]);
            log << endl;
        }

        // Close the file
        out.close();
        log.close();
    }
}

namespace smallest_square
{
    using gen_quad::Point;

    class Square
    {
    public:
        Square()
        {
        }
        Square(Point p1, Point p2, Point p3, Point p4) : p1(p1), p2(p2), p3(p3), p4(p4)
        {
        }
        double calc_area()
        {
            double dist = this->p1.calc_dist(this->p2);
            return dist * dist;
        }
        Point get_p1()
        {
            return this->p1;
        }
        Point get_p2()
        {
            return this->p2;
        }
        Point get_p3()
        {
            return this->p3;
        }
        Point get_p4()
        {
            return this->p4;
        }

    private:
        Point p1, p2, p3, p4;
    };

    bool read_points(Point points[4])
    {
        ifstream point_file("points.txt");
        string nums;
        char line[50];
        double x, y;

        if (!point_file.is_open() & !point_file.getline(line, 50, '('))
        {
            return false;
        }

        for (int i = 0; i < 4; i++)
        {
            if (!point_file.getline(line, 50, '('))
            {
                return false;
            }
            nums = string(line);
            x = stod(nums.substr(0, 19));
            y = stod(nums.substr(20, 19));
            points[i] = ppm::Point(x, y);
        }
        point_file.close();

        return true;
    }

    void gen_squares(Point points[4], Square *s1, Square *s2)
    {
    }
}

int main()
{
    // Gen quad
    gen_quad::gen_quad();
    // Read points
    ppm::Point points[4];
    smallest_square::read_points(points);
    // Gen all 6 squares
    // Find smallest
    // Output stuff
    ppm::Image *image = new ppm::Image("output.ppm");

    delete image;
    return 0;
}