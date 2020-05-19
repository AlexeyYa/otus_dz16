
#ifndef ENTRY_H
#define ENTRY_H

#include <iostream>
#include <dlib/clustering.h>

struct Entry
{
    Entry(){}
    Entry(double latitude, double longitude, int roomcount, double price, double area,
          double kitchen_area, int floor, int total_floor) :
        latitude(latitude), longitude(longitude), roomcount(roomcount), price(price),
        area(area), kitchen_area(kitchen_area), floor(floor), total_floor(total_floor)
    {}
    double latitude;
    double longitude;
    int roomcount;
    double price;
    double area;
    double kitchen_area;
    int floor;
    int total_floor;

    double SqaredDist(const dlib::matrix<double,7,1>& sample, const std::vector<dlib::matrix<double,7,1>>& minmax) const
    {
        double res = ((latitude - minmax[0](0)) / (minmax[1](0) - minmax[0](0)) - sample(0)) *
                ((latitude - minmax[0](0)) / (minmax[1](0) - minmax[0](0)) - sample(0));
        res += ((longitude - minmax[0](1)) / (minmax[1](1) - minmax[0](1)) - sample(1)) *
                ((longitude - minmax[0](1)) / (minmax[1](1) - minmax[0](1)) - sample(1));
        res += ((roomcount - minmax[0](2)) / (minmax[1](2) - minmax[0](2)) - sample(2)) *
                ((roomcount - minmax[0](2)) / (minmax[1](2) - minmax[0](2)) - sample(2));
        res += ((price - minmax[0](3)) / (minmax[1](3) - minmax[0](3)) - sample(3)) *
                ((price - minmax[0](3)) / (minmax[1](3) - minmax[0](3)) - sample(3));
        res += ((area - minmax[0](4)) / (minmax[1](4) - minmax[0](4)) - sample(4)) *
                ((area - minmax[0](4)) / (minmax[1](4) - minmax[0](4)) - sample(4));
        res += ((kitchen_area - minmax[0](5)) / (minmax[1](5) - minmax[0](5)) - sample(5)) *
                ((kitchen_area - minmax[0](5)) / (minmax[1](5) - minmax[0](5)) - sample(5));
        res += (floor == 1 || floor == total_floor) == sample(6);
        return res;
    }
};

std::ostream& operator<<(std::ostream& os, const Entry& ent)
{
    os << ent.latitude << ';'
       << ent.longitude << ';'
       << ent.roomcount << ';'
       << ent.price << ';'
       << ent.area << ';'
       << ent.kitchen_area << ';'
       << ent.floor << ';'
       << ent.total_floor;
    return os;
}

namespace dlib
{
    void serialize(const Entry& ent, std::ostream& out)
    {
        try
        {
            serialize(ent.latitude, out);
            serialize(ent.longitude, out);
            serialize(ent.roomcount, out);
            serialize(ent.price, out);
            serialize(ent.area, out);
            serialize(ent.kitchen_area, out);
            serialize(ent.floor, out);
            serialize(ent.total_floor, out);
        }
        catch (serialization_error& e)
        {
            throw serialization_error(e.info + "\n   while serializing object of type Entry");
        }
    }

    void deserialize(Entry& ent, std::istream& in)
    {
        try
        {
            deserialize(ent.latitude, in);
            deserialize(ent.longitude, in);
            deserialize(ent.roomcount, in);
            deserialize(ent.price, in);
            deserialize(ent.area, in);
            deserialize(ent.kitchen_area, in);
            deserialize(ent.floor, in);
            deserialize(ent.total_floor, in);
        }
        catch (serialization_error& e)
        {
            throw serialization_error(e.info + "\n   while deserializing object of type Entry");
        }
    }
}
#endif
