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
        Line() {}
        Line(double x1, double y1, double x2, double y2) : x1(x1), x2(x2), y1(y1), y2(y2) {}
        Line(Point p1, Point p2) : x1(p1.xpos()), x2(p2.xpos()), y1(p1.ypos()), y2(p2.ypos()) {}
        Line(double x, double y, double slope) : x1(x), y1(y)
        {
            this->x2 = x + 1;
            this->y2 = y + slope;
            this->extend(0, 1);
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
        auto print_point = [&](Point &point)
        { log << "(" << point.xpos() << ", " << point.ypos() << ")"; };

        log << "Inital Points: ";
        for (int i = 0; i < 4; i++)
        {
            p[i] = Point(xy(gen), xy(gen));
            log << "p" << i << " = ";
            print_point(p[i]);
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
                print_point(p[3]);
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
            print_point(p[i]);
            log << endl;
        }

        // Close the file
        out.close();
        log.close();
    }
}

namespace small
{
    using gen_quad::Point;
    using ppm::Circle;
    using ppm::Image;
    using ppm::Line;

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
            int m_ind = nums.find_first_of(',');
            int e_ind = nums.find_first_of(')');
            x = stod(nums.substr(0, m_ind));
            y = stod(nums.substr(m_ind + 1, e_ind - m_ind - 1));
            points[i] = ppm::Point(x, y);
        }
        point_file.close();

        return true;
    }

    void gen_squares(Point points[4], Square &s1, Square &s2)
    {
        auto gen_square = [&](Point e)
        {
            double slope = (e.ypos() - points[3].ypos()) / (e.xpos() - points[3].xpos());
            double perp_slope = -1 / slope;
            Line bottom(points[3].xpos(), points[3].ypos(), slope);
            Line top(points[2].xpos(), points[2].ypos(), slope);
            Line left(points[0].xpos(), points[0].ypos(), perp_slope);
            Line right(points[1].xpos(), points[1].ypos(), perp_slope);
            Point p1 = bottom.calc_intersect(left);
            Point p2 = bottom.calc_intersect(right);
            Point p3 = top.calc_intersect(left);
            Point p4 = top.calc_intersect(right);
            return Square(p1, p2, p3, p4);
        };
        // Assume E is below points[2]
        // x = x3 - y2 + y1;
        // y = y3 + x2 - x1;
        Point e1 = Point(points[2].xpos() - points[1].ypos() + points[0].ypos(), points[2].ypos() + points[1].xpos() - points[0].xpos());
        s1 = gen_square(e1);
        // Assume E is above points[2]
        // x = x3 + y2 - y1;
        // y = y3 - x2 + x1;
        Point e2 = Point(points[2].xpos() + points[1].ypos() - points[0].ypos(), points[2].ypos() - points[1].xpos() + points[0].xpos());
        s2 = gen_square(e2);
    }

    void output_smallest_square()
    {
        Point points[4];
        Square squares[6];
        Line l1, l2, l3, l4;
        int smallest = 0;
        double area = 0;
        Image *image = new Image("output.ppm");
        ofstream output("output.txt");
        output << fixed << setprecision(17);
        auto print_point = [&](Point &point)
        { output << "(" << point.xpos() << "," << point.ypos() << ")"; };
        auto print_square = [&](Square &square)
        {
            output << "(" << square.get_p1().xpos() << "," << square.get_p1().ypos() << ")";
            output << " , ";
            output << "(" << square.get_p2().xpos() << "," << square.get_p2().ypos() << ")";
            output << " , ";
            output << "(" << square.get_p3().xpos() << "," << square.get_p3().ypos() << ")";
            output << " , ";
            output << "(" << square.get_p4().xpos() << "," << square.get_p4().ypos() << ")";
            output << " Area = " << square.calc_area() << endl;
        };
        // Read points
        read_points(points);
        // Print points
        for (int i = 0; i < 4; i++)
        {
            // Print Points
            print_point(points[i]);
            if (i != 3)
            {
                output << " , ";
            }
            else
            {
                output << endl;
            }
        }
        // Gen all 6 squares
        for (int i = 0; i < 3; i++)
        {
            Point ps[] = {points[3], points[(i + 0) % 3], points[(i + 1) % 3], points[(i + 2) % 3]};
            gen_squares(ps, squares[i * 2], squares[i * 2 + 1]);
        }
        // Find smallest
        for (int i = 0; i < 6; i++)
        {
            double a = squares[i].calc_area();
            if (a > area)
            {
                smallest = i;
                area = a;
            }
            // Output square stuff
            print_square(squares[i]);
        }
        auto ssquare = squares[smallest];
        // Draw squares
        l1 = Line(ssquare.get_p1(), ssquare.get_p2());
        l1.extend(0, 1);
        l1.draw(image, 0, 255, 0);
        l2 = Line(ssquare.get_p2(), ssquare.get_p4());
        l2.extend(0, 1);
        l2.draw(image, 0, 255, 125);
        l3 = Line(ssquare.get_p3(), ssquare.get_p4());
        l3.extend(0, 1);
        l3.draw(image, 0, 255, 255);
        l4 = Line(ssquare.get_p3(), ssquare.get_p1());
        l4.extend(0, 1);
        l4.draw(image, 255, 255, 0);
        // Draw square vertices
        Circle(ssquare.get_p1().xpos(), ssquare.get_p1().ypos(), 2.0 / 800.0).draw(image, 255, 0, 0);
        Circle(ssquare.get_p2().xpos(), ssquare.get_p2().ypos(), 2.0 / 800.0).draw(image, 255, 0, 0);
        Circle(ssquare.get_p3().xpos(), ssquare.get_p3().ypos(), 2.0 / 800.0).draw(image, 255, 0, 0);
        Circle(ssquare.get_p4().xpos(), ssquare.get_p4().ypos(), 2.0 / 800.0).draw(image, 255, 0, 0);
        // Draw points
        for (int i = 0; i < 4; i++)
        {
            Circle(points[i].xpos(), points[i].ypos(), 3.0 / 800.0).draw(image, 139, 0, 139);
        }
        // Output PPM
        image->output();
        // Close resources
        delete image;
    }
}

int main()
{
    // Gen quad
    gen_quad::gen_quad();
    // Smallest square
    small::output_smallest_square();
    //
    return 0;
}