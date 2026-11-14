#pragma once

#include "osmfeature.hpp"

class GLWidget;

class FeatureDispatcher {
public:
  explicit FeatureDispatcher(GLWidget *widget);

  void dispatch(const OSMFeature &feature);

  void dispatchAll(const QVector<OSMFeature> &features);

private:
  GLWidget *m_widget = nullptr;

  void dispatchBuilding(const OSMFeature &feature);
  void dispatchRoad(const OSMFeature &feature);
  void dispatchWater(const OSMFeature &feature);
  void dispatchVegetation(const OSMFeature &feature);
  void dispatchLanduse(const OSMFeature &feature);
  void dispatchRailway(const OSMFeature &feature);
  void dispatchAmenity(const OSMFeature &feature);
};