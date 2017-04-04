/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 * @flow
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  View
} from 'react-native';

export default class ArdyMedReact extends Component {
  render() {
    return (
		<View style = {styles.outerContainer}>
			<View style = {styles.circleContainer}>
				<Text style = {styles.logo}>
				AM 
				</Text>
			</View>
			<View style = {styles.bottomContainer}>
				<Text style = {styles.appName}>
				ArdyMed 
				</Text>
				<Text style = {styles.slogan}>
				Lets Make {'\n'}
				Diabetes Easy 
				</Text> 
			</View>
		</View>
    );
  }
}

const styles = StyleSheet.create({
  outerContainer: {
		flex: 1,
		alignItems: 'center',
		backgroundColor: '#FFFFFF',
	},
	circleContainer: {
		height: 200,
		width: 200,
		borderRadius: 100,
		marginTop: 100,
		zIndex: 5,
		backgroundColor: '#FFFFFF',
	},
	bottomContainer: {
		flex: 1,
		width: '100%',
		marginTop: -80,
		zIndex: 1,
		backgroundColor: '#3EE699'
	},
	logo: {
		fontSize: 80,
		textAlign: 'center',
		marginTop: 70,
		color: '#3EE699',
   		backgroundColor: 'rgba(0,0,0,0)',
	},
	appName: {
		fontSize: 30,
		textAlign: 'center',
		color: '#FFFFFF',
		marginTop: 130,
		marginBottom: 5,
	},
	slogan: {
		fontSize: 18,
		textAlign: 'center',
		color: '#FFFFFF',
	},
});

AppRegistry.registerComponent('ArdyMedReact', () => ArdyMedReact);
