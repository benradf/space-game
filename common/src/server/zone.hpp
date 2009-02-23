#ifndef ZONE_HPP
#define ZONE_HPP


// class Position {
//     public:
//         virtual int x() = 0;
//         virtual int y() = 0;
//         virtual int z() = 0;
//         
//     private:
//         
// };
// 
// class Position2D : public Position {
// 
// };
// 
// class Position3D : public Position {
// 
// };
// 
// 
// class Object {
// 
// };
// 
// class ObjectVisitor {
//     public:
//         virtual void visit(Object& object) = 0;
// };
// 
// class Zone {
//     public:
//         void attach(Zone* zone);
//         virtual int cost() = 0;
//         virtual int objects() = 0;
//         virtual void rebalance() = 0;
//         virtual void accept(ObjectVisitor& visitor) = 0;
//         virtual void intersect(Rectangle& area, CallBack& callback);
//         
//     private:
//         
// };
// 
// class ZoneBranch : public Zone {
//     public:
//         virtual ZoneBranch* create() = 0;  // virtual constructor
//         
//     private:
//         
// };
// 
// class QuadBranch : public ZoneBranch {
//     public:
//         
//     private:
//         Zone* _quad1;
//         Zone* _quad2;
//         Zone* _quad3;
//         Zone* _quad4;
// };
// 
// class CubeBranch : public ZoneBranch {
//     public:
//         
//     private:
//         Zone* _cube1;
//         Zone* _cube2;
//         Zone* _cube3;
//         Zone* _cube4;
//         Zone* _cube5;
//         Zone* _cube6;
//         Zone* _cube7;
//         Zone* _cube8;
// };
// 
// 
// class VirtualZone : public Zone {
// 
// };
// 
// class LocalZone : public Zone {
//     public:
//     
//     private:
//         std::vector<Object> _objects;  // <--- something like this
// };
// 
// class RemoteZone : public Zone {
//     
// };



// Parent branches split their children when they contain too many objects. In
// this manner they are able to replace the children with another branch of the
// same type as themselves and distribute the objects between the new branch's
// children.



#endif  // ZONE_HPP
