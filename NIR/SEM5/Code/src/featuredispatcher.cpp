#include "featuredispatcher.hpp"
#include "glwidget.hpp"

#include <QDebug>

FeatureDispatcher::FeatureDispatcher(GLWidget *widget) : m_widget(widget) {}

void FeatureDispatcher::dispatch(const OSMFeature &feature) {
  if (!m_widget) {
    qWarning() << "FeatureDispatcher: GLWidget is null";
    return;
  }

  if (feature.geometry.isEmpty())
    return;

  switch (feature.type) {
  case OSMFeatureType::Building:
    dispatchBuilding(feature);
    break;

  case OSMFeatureType::Road:
    dispatchRoad(feature);
    break;

  case OSMFeatureType::Water:
    dispatchWater(feature);
    break;

  case OSMFeatureType::Vegetation:
    dispatchVegetation(feature);
    break;

  case OSMFeatureType::Landuse:
    dispatchLanduse(feature);
    break;

  case OSMFeatureType::Railway:
    dispatchRailway(feature);
    break;

  case OSMFeatureType::Amenity:
    dispatchAmenity(feature);
    break;

  case OSMFeatureType::Unknown:
  default:
    break;
  }
}

void FeatureDispatcher::dispatchAll(const QVector<OSMFeature> &features) {
  for (const OSMFeature &feature : features)
    dispatch(feature);
}

void FeatureDispatcher::dispatchBuilding(const OSMFeature &feature) {
  if (feature.geometryType == OSMGeometryType::Polygon)
    m_widget->addBuildingFeature(feature);
}

void FeatureDispatcher::dispatchRoad(const OSMFeature &feature) {
  if (feature.geometryType == OSMGeometryType::LineString)
    m_widget->addRoadFeature(feature);
  else if (feature.geometryType == OSMGeometryType::Polygon)
    m_widget->addRoadAreaFeature(feature);
}

void FeatureDispatcher::dispatchWater(const OSMFeature &feature) {
  if (feature.geometryType == OSMGeometryType::Polygon)
    m_widget->addWaterAreaFeature(feature);
  else if (feature.geometryType == OSMGeometryType::LineString)
    m_widget->addWaterLineFeature(feature);
}

void FeatureDispatcher::dispatchVegetation(const OSMFeature &feature) {
  if (feature.geometryType == OSMGeometryType::Polygon)
    m_widget->addVegetationAreaFeature(feature);
  else if (feature.geometryType == OSMGeometryType::Point)
    m_widget->addVegetationPointFeature(feature);
}

void FeatureDispatcher::dispatchLanduse(const OSMFeature &feature) {
  if (feature.geometryType == OSMGeometryType::Polygon)
    m_widget->addLanduseAreaFeature(feature);
}

void FeatureDispatcher::dispatchRailway(const OSMFeature &feature) {
  if (feature.geometryType == OSMGeometryType::LineString)
    m_widget->addRailwayFeature(feature);
}

void FeatureDispatcher::dispatchAmenity(const OSMFeature &feature) {
  if (feature.geometryType == OSMGeometryType::Point)
    m_widget->addAmenityPointFeature(feature);
  else if (feature.geometryType == OSMGeometryType::Polygon)
    m_widget->addAmenityAreaFeature(feature);
}