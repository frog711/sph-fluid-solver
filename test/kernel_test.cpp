#include <gtest/gtest.h>
#include "../src/structures.hpp"
#include "../src/simulate.hpp"
#include "../src/kernel.hpp"

// Demonstrate some basic assertions.
TEST(NeighborSearch, regular) {
  structures::config conf = {0, true, 2, 0, 2, {}, {}};

  auto simulator = simulate::Simulator(conf);
  conf = simulator.parseFile("../data/regular");
  kernel::Kernel kernel = kernel::Kernel(conf);
  kernel.initialize(simulator.getParticleData());
  kernel.completeNeighborSearch();
  for (int i = 0; i < simulator.getParticles().size(); i++) {
    if (i % 10 != 0 && i % 10 != 9 && i > 9 && i < 90) {
      EXPECT_EQ(simulator.getParticles()[i].neighbors.size(), 9);
    }
  }
}

TEST(NeighborSearch, angled) {
  structures::config conf = {0, true, 2, 0, 2, {}, {}};

  auto simulator = simulate::Simulator(conf);
  conf = simulator.parseFile("../data/angled");
  kernel::Kernel kernel = kernel::Kernel(conf);
  kernel.initialize(simulator.getParticleData());
  kernel.completeNeighborSearch();
  for (int i = 0; i < simulator.getParticles().size(); i++) {
    if (i % 10 != 0 && i % 10 != 9 && i > 9 && i < 90) {
      EXPECT_EQ(simulator.getParticles()[i].neighbors.size(), 9);
    }
  }
}
/*
TEST(NeighborSearch, large) {
  structures::config conf = {true, 2, 20, 1.9, {0, 9.81}, {500, 500}};

  auto simulator = simulate::Simulator(conf);
  simulator.parseFile("../data/large");
  kernel::Kernel kernel = kernel::Kernel(conf);
  kernel.initialize(simulator.getParticleData(), simulator.getParticles().size());
  kernel.completeNeighborSearch();
  for (int i = 0; i < simulator.getParticles().size(); i++) {
    if (i % 50 != 0 && i % 50 != 49 && i > 50 && i < 2450) {
      EXPECT_EQ(simulator.getParticles()[i].neighbors.size(), 9);
    }
  }
}
  */

TEST(Kernel, regular) {
  structures::config conf = {0, true, 2, 0, 2, {}, {}};
  auto simulator = simulate::Simulator(conf);
  conf = simulator.parseFile("../data/regular");
  kernel::Kernel kernel = kernel::Kernel(conf);
  kernel.initialize(simulator.getParticleData());
  kernel.completeNeighborSearch();
  
  kernel.calculateKernel();
  double vInv = 1.0 / 400.0;
  for (int i = 0; i < simulator.getParticles().size(); i++) {
    if (i % 10 != 0 && i % 10 != 9 && i > 9 && i < 90) {
      double sum = 0;
      for (int k = 0; k < simulator.getParticles()[i].neighbors.size(); k++) {
        int j = simulator.getParticles()[i].neighbors[k];
        sum = sum + kernel.getKernelEntry(i, j);
        if (i == 35) std::cout << "Found " << i << "," << j << ": " <<kernel.getKernelEntry(i, j) << "\n";
      }
      double error = std::abs(vInv - sum) / vInv;
      EXPECT_TRUE(error < 0.001);
    }
  }
}

TEST(Kernel, angled) {
  structures::config conf = {0, true, 2, 0, 2, {}, {}};
  auto simulator = simulate::Simulator(conf);
  conf = simulator.parseFile("../data/angled");
  kernel::Kernel kernel = kernel::Kernel(conf);
  kernel.initialize(simulator.getParticleData());
  kernel.completeNeighborSearch();
  
  kernel.calculateKernel();
  double vInv = 1.0 / 400.0;
  for (int i = 0; i < simulator.getParticles().size(); i++) {
    if (i % 10 != 0 && i % 10 != 9 && i > 9 && i < 90) {
      double sum = 0;
      for (int k = 0; k < simulator.getParticles()[i].neighbors.size(); k++) {
        int j = simulator.getParticles()[i].neighbors[k];
        sum = sum + kernel.getKernelEntry(i, j);
        if (i == 35) std::cout << "Found " << i << "," << j << ": " <<kernel.getKernelEntry(i, j) << "\n";
      }
      double error = std::abs(vInv - sum) / vInv;
      EXPECT_TRUE(error < 0.001);
    }
  }
}

TEST(KernelGrad, regular) {
  structures::config conf = {0, true, 2, 0, 2, {}, {}};
  auto simulator = simulate::Simulator(conf);
  conf = simulator.parseFile("../data/regular");
  kernel::Kernel kernel = kernel::Kernel(conf);
  kernel.initialize(simulator.getParticleData());
  kernel.completeNeighborSearch();
  
  kernel.calculateKernelDerivative();
  double vInv = 1.0 / 400.0;
  for (int i = 0; i < simulator.getParticles().size(); i++) {
    if (i % 10 != 0 && i % 10 != 9 && i > 9 && i < 90) {
      double sum1 = 0;
      double sum2 = 0;
      double crossProduct11 = 0;
      double crossProduct12 = 0;
      double crossProduct21 = 0;
      double crossProduct22 = 0;
      for (int k = 0; k < simulator.getParticles()[i].neighbors.size(); k++) {
        int j = simulator.getParticles()[i].neighbors[k];
        EXPECT_EQ(kernel.getDerivativeEntry(i, j)[0], -1 * kernel.getDerivativeEntry(j, i)[0]);
        EXPECT_EQ(kernel.getDerivativeEntry(i, j)[1], -1 * kernel.getDerivativeEntry(j, i)[1]);
        double derivx = kernel.getDerivativeEntry(i, j)[0];
        double derivy = kernel.getDerivativeEntry(i, j)[1];
        double diffx = simulator.getParticles()[i].pos[0] - simulator.getParticles()[j].pos[0];
        double diffy = simulator.getParticles()[i].pos[1] - simulator.getParticles()[j].pos[1];
        sum1 = sum1 + derivx;
        sum2 = sum2 + derivy;
        crossProduct11 += diffx * derivx;
        crossProduct12 += diffx * derivy;
        crossProduct21 += diffy * derivx;
        crossProduct22 += diffy * derivy;
        if (i == 35) std::cout << "Found " << i << "," << j << ": " <<kernel.getKernelEntry(i, j) << "\n";
      }
      double errorx = std::abs(-1 * vInv - crossProduct11) / (vInv);
      double errory = std::abs(-1 * vInv - crossProduct22) / (vInv);
      EXPECT_TRUE(std::abs(sum1) < 0.0000000001);
      EXPECT_TRUE(std::abs(sum2) < 0.0000000001);
      EXPECT_TRUE(std::abs(crossProduct12) < 0.0000000001);
      EXPECT_TRUE(std::abs(crossProduct21) < 0.0000000001);
      EXPECT_TRUE(errorx < 0.02);
      EXPECT_TRUE(errory < 0.02);
    }
  }
}

TEST(KernelGrad, angled) {
  structures::config conf = {0, true, 2, 0, 2, {}, {}};
  auto simulator = simulate::Simulator(conf);
  conf = simulator.parseFile("../data/angled");
  kernel::Kernel kernel = kernel::Kernel(conf);
  kernel.initialize(simulator.getParticleData());
  kernel.completeNeighborSearch();
  
  kernel.calculateKernelDerivative();
  double vInv = 1.0 / 400.0;
  for (int i = 0; i < simulator.getParticles().size(); i++) {
    if (i % 10 != 0 && i % 10 != 9 && i > 9 && i < 90) {
      double sum1 = 0;
      double sum2 = 0;
      double crossProduct11 = 0;
      double crossProduct12 = 0;
      double crossProduct21 = 0;
      double crossProduct22 = 0;
      for (int k = 0; k < simulator.getParticles()[i].neighbors.size(); k++) {
        int j = simulator.getParticles()[i].neighbors[k];
        EXPECT_EQ(kernel.getDerivativeEntry(i, j)[0], -1 * kernel.getDerivativeEntry(j, i)[0]);
        EXPECT_EQ(kernel.getDerivativeEntry(i, j)[1], -1 * kernel.getDerivativeEntry(j, i)[1]);
        double derivx = kernel.getDerivativeEntry(i, j)[0];
        double derivy = kernel.getDerivativeEntry(i, j)[1];
        double diffx = simulator.getParticles()[i].pos[0] - simulator.getParticles()[j].pos[0];
        double diffy = simulator.getParticles()[i].pos[1] - simulator.getParticles()[j].pos[1];
        sum1 = sum1 + derivx;
        sum2 = sum2 + derivy;
        crossProduct11 += diffx * derivx;
        crossProduct12 += diffx * derivy;
        crossProduct21 += diffy * derivx;
        crossProduct22 += diffy * derivy;
        if (i == 35) std::cout << "Found " << i << "," << j << ": " <<kernel.getKernelEntry(i, j) << "\n";
      }
      double errorx = std::abs(-1 * vInv - crossProduct11) / (vInv);
      double errory = std::abs(-1 * vInv - crossProduct22) / (vInv);
      EXPECT_TRUE(std::abs(sum1) < 0.0000000001);
      EXPECT_TRUE(std::abs(sum2) < 0.0000000001);
      EXPECT_TRUE(std::abs(crossProduct12) < 0.0000000001);
      EXPECT_TRUE(std::abs(crossProduct21) < 0.0000000001);
      EXPECT_TRUE(errorx < 0.02);
      EXPECT_TRUE(errory < 0.02);
    }
  }
}
