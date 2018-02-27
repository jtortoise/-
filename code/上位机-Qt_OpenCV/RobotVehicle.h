#ifndef __ROBOT_VEHICLE_H
#define __ROBOT_VEHICLE_H

#include <vector>
#include "opencv2/core.hpp"
#include "opencv2/calib3d.hpp"

class RobotVehicle
{
public:
	RobotVehicle(const cv::Mat &cameraMatrix, int markerID=0);
	~RobotVehicle();
	
	void setMarkerID(int markerID);
	void setTarget(const cv::Point2d & target);

	cv::Point3f updateDeviation(const std::vector<cv::Point2f> & markerCorners, const cv::Vec3d & rvector, const cv::Vec3d & tvector);
	const cv::Point2f& position();
protected:
	cv::Mat m_cameraMatrix;
	//Target�ǻ�������ͷ����ϵ�����꣬��ͼ������
	cv::Point3d m_lastTarget;
	//Deviation.x��RobotVehicle��Target�����λ��(��λm)
	//Deviation.y��RobotVehicle��Target�����Ƕ�(��λ��)
	//Deviation.z��RobotVehicle��ǰ�����ķ���(��λ�ǡ�)
	cv::Point3f m_lastDeviation;
	//���RobotVehicle��MarkerID
	int m_markerID;
	//marker��������ͷ����ϵ����������
	cv::Point2f m_self;

private:

};

#endif

