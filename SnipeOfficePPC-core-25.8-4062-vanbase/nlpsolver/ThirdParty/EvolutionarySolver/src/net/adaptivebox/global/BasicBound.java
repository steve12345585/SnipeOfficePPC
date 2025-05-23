/**
 * Description: provide an bound, and the corresponding operations
 *
 * @ Author        Create/Modi     Note
 * Xiaofeng Xie    Oct. 9, 2002
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * Please acknowledge the author(s) if you use this code in any way.
 */

package net.adaptivebox.global;

public class BasicBound {
  public static final double MINDOUBLE= -1e308;
  public static final double MAXDOUBLE= 1e308;

  public double minValue = MINDOUBLE;
  public double maxValue = MAXDOUBLE;
  public BasicBound() {
  }

  public BasicBound(double min, double max) {
    minValue = Math.min(min, max);
    maxValue = Math.max(min, max);
  }

  public double getLength() {
    return Math.abs(maxValue-minValue);
  }

  public boolean isSatisfyCondition(double child){
    if(child > maxValue || child < minValue) {
      return(false);
    }
    return(true);
  }

  public double boundAdjust(double value){
    if(value > maxValue) {
      value = maxValue;
    } else if (value < minValue) {
      value = minValue;
    }
    return value;
  }

  public double annulusAdjust (double value) {
    if(value > maxValue) {
      double extendsLen = (value-maxValue)%getLength();
      value = minValue+extendsLen;
    } else if (value < minValue) {
      double extendsLen = (minValue-value)%getLength();
      value = maxValue-extendsLen;
    }
    return value;
  }

  public static BasicBound getBound(double[] data) {
    BasicBound bound = new BasicBound();
    if(data!=null) {
      double minV, maxV;
      if(data.length>0) {
        bound.minValue = data[0];
        bound.maxValue = data[0];
        for(int i=1; i<data.length; i++) {
          bound.minValue = Math.min(bound.minValue, data[i]);
          bound.maxValue = Math.max(bound.maxValue, data[i]);
        }

      }
    }
    return bound;
  }

  public double randomAdjust (double value){
    if(value > maxValue || value < minValue) {
      value =  getRandomValue();
    }
    return value;
  }

  public double getRandomValue(){
    return RandomGenerator.doubleRangeRandom(minValue, maxValue);
  }
}