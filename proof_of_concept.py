from math import sin, cos

def create(*args):
    return [a for a in args]

def dot(lhs, rhs):
    return sum([a * b for a, b in zip(lhs, rhs)])

# def cross(lhs, rhs):
#     return [lhs[1] * rhs[2] - lhs[2] * rhs[1],
#             -(lhs[0] * rhs[2] - lhs[2] * rhs[0]),
#             lhs[0] * rhs[1] - lhs[1] * rhs[0]]

class Point:
    
    def __init__(self, *args):
        self.points = list(args)
        
    def __add__(self, rhs):
        temp = self.points
        if isinstance(temp, float):
            for i in range(len(temp)):
                temp[i] += rhs
        if isinstance(temp, list):
            for i in range(len(temp)):
                temp[i] += rhs[i]
        return temp
    
    def __sub__(self, rhs):
        temp = self.points
        if isinstance(rhs, float):
            for i in range(len(temp)):
                temp[i] -= rhs
        elif isinstance(rhs, list):
            for i in range(len(temp)):
                temp[i] -= rhs[i]
        return temp
    
    def __mul__(self, rhs):
        temp = self.points
        if isinstance(rhs, float):
            for i in range(len(temp)):
                temp[i] *= rhs
                return temp
        elif isinstance(rhs, list):
            for i in range(len(temp)):
                temp[i] *= rhs[i]
                return temp

class Mat:
    def __init__(self, _matrix=[[1, 0, 0],
                                [0, 1, 0],
                                [0, 0, 1]]):
        self.matrix = _matrix
    def __mul__(self, rhs):

        pass


def __mut__(lhs, rhs):
    if lhs == rhs:
        return lhs * rhs
    if rhs == type(Point):
        return [lhs * rhs[i] for i in range(rhs)]

class Plane:
    pass

# create two cubes
class Cube:
    def __init__(self, _center, _triangles, _normal, _mass=1, _e=0.4):
        self.center = _center
        self.triangles = _triangles
        self.normal = _normal
        self.mass = _mass
        self.e = _e

# 0 1
# 2 3
A = Cube(Point(0, 2.5, 0),
         [Point(0, 3, 2), Point(0, 1, 3)],
         Point(0, -1, 0))

B = Cube(Point(0, 0, 0), [Point(0, 3, 2), Point(0, 1, 3)], Point(0, 1, 0))

# acc
dt = 0.00167
g = Point(0, -9.806, 0)

# move towards ground
def update():
    print(f"{A.center=}\t{B.center=}")
    A.center -= g * dt
    B.center = 0



if __name__ == "__main__":
    while True:
        # response()
        update()
        input()

# calc colision
def colRes(A, B) -> list:
    
    # ...
    
    # B
    
    # momentum calc
    #   \
    #    \
    # ----\
        
        
    #
    #
    #
    #  ______
    # |     |
    # |     |
    # |     |
    # |_____|
    #
    normal = Point(0.707, 0.707)
    center = Point (3, 3)

    

  
