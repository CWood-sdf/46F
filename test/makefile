sp :=
sp +=
qs = $(subst ?,$(sp),$1)
sq = $(subst $(sp),?,$1)

INC = -I"C:/Users/woodc/source/VEX CODE/46F/include/Odometry" -I"C:/Users/woodc/source/VEX CODE/46F/test/include"
CXX = clang
test:
	$(CXX) test.cpp -DTEST $(INC) -std=c++17
