sp :=
sp +=
qs = $(subst ?,$(sp),$1)
sq = $(subst $(sp),?,$1)

INC = -I"C:/Users/woodc/source/46X/include/Odometry" -I"C:/Users/woodc/source/46X/test/include"
CXX = clang
test:
	$(CXX) test.cpp -DTEST $(INC) -std=c++17
