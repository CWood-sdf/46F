sp :=
sp +=
qs = $(subst ?,$(sp),$1)
sq = $(subst $(sp),?,$1)

INC = -I"C:/Users/Student.EGR_03_LAPTOP.001/46F/include/Odometry" -I"C:/Users/Student.EGR_03_LAPTOP.001/46F/test/include"
CXX = clang
test:
	$(CXX) test.cpp -DTEST $(INC) -std=c++17
